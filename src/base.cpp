//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/base.h>
#include <algorithm/events.h>
#include <algorithm/isolution.h>
#include <ctoolhu/event/firer.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;

namespace Algorithm {

AlgorithmBase::AlgorithmBase()
:
	_stopRequested{false},
	_paused{false},
	_parent{nullptr}
{
}

void AlgorithmBase::SetParent(const IAlgorithm *parent)
{
	_parent = parent;
}

Fitness AlgorithmBase::GetWorstFitness()
{
	return Fitness{numeric_limits<Fitness::id_type>::max()};
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
	_Normalize(solutionPtr);
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

void AlgorithmBase::_Normalize(const solution_ptr_type &solutionPtr) const
{
	bool changed = solutionPtr->Normalize();
	solutionPtr->GetFitness(); //to make sure it's initialized
	if (changed) {
		//results of the normalization must always be accepted
		Ctoolhu::Event::Fire(Algorithm::Events::BestSolutionFound { solutionPtr.get(), ElapsedTime() });
		if (solutionPtr->IsFeasible())
			Ctoolhu::Event::Fire(Algorithm::Events::FeasibleSolutionFound { solutionPtr.get(), ElapsedTime() });
	}
}

void AlgorithmBaseConfig::Load(const boost::property_tree::ptree &pt)
{
	keepFeasible = pt.get("keepFeasible", false);
	extended = pt.get("extended", false);
	benchmark = pt.get("benchmark", false);
}

void AlgorithmBaseConfig::Propagate(boost::property_tree::ptree &dst)
{
	dst.add("keepFeasible", keepFeasible);
	dst.add("extended", extended);
	dst.add("benchmark", benchmark);
}

} //ns Algorithm
