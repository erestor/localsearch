//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/base.h>
#include <algorithm/events.h>
#include <algorithm/isolution.h>
#include <ctoolhu/event/firer.hpp>

using namespace std;

namespace Algorithm {

AlgorithmBase::AlgorithmBase()
:
	_stopRequested(false),
	_paused(false),
	_parent(nullptr)
{
}

void AlgorithmBase::SetParent(const IAlgorithm *parent)
{
	_parent = parent;
}

const Fitness &AlgorithmBase::GetWorstFitness() const
{
	static Fitness worst { numeric_limits<Fitness::id_type>::max() };
	return worst;
}

void AlgorithmBase::_TogglePause(bool pause)
{
	unique_lock lock(_pauseMutex);
    _paused = pause;
	lock.unlock(); //manual unlocking is done before notifying... (from C++ reference)
    _pauseChanged.notify_all();
}

void AlgorithmBase::Pause()
{
	_TogglePause(true);
}

void AlgorithmBase::Resume()
{
	_TogglePause(false);
}

bool AlgorithmBase::Start(const solution_ptr_type &solutionPtr)
{
	_stopRequested = false;
	_timer.StartClock();
	solutionPtr->GetFitness(); //to make sure it's initialized
	Ctoolhu::Event::Fire(Events::Started { solutionPtr.get(), Name() });
	auto result = Run(solutionPtr);
	Ctoolhu::Event::Fire(Events::Finished { solutionPtr.get(), Name() });
	return result;
}

void AlgorithmBase::Stop()
{
	lock_guard _(_pauseMutex);
	_stopRequested = true;
}

bool AlgorithmBase::IsStopRequested() const
{
	unique_lock lock(_pauseMutex);

	//if paused, don't allow continuation until resumed
	while (_paused && !_stopRequested)
		_pauseChanged.wait(lock);

	return _stopRequested || (_parent && _parent->IsStopRequested());
}

chrono::milliseconds AlgorithmBase::ElapsedTime() const
{
	return _timer.ElapsedTime();
}

} //ns Algorithm