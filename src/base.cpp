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

void AlgorithmBase::setParent(const IAlgorithm *parent)
{
	_parent = parent;
}

void AlgorithmBase::_togglePause(bool pause)
{
	unique_lock lock(_pauseMutex);
    _paused = pause;
	lock.unlock(); //manual unlocking is done before notifying... (from C++ reference)
    _pauseChanged.notify_all();
}

void AlgorithmBase::pause()
{
	_togglePause(true);
}

void AlgorithmBase::resume()
{
	_togglePause(false);
}

bool AlgorithmBase::start(solution_ptr_t solutionPtr)
{
	_stopRequested = false;
	_timer.StartClock();
	Ctoolhu::Event::Fire(Events::Started { solutionPtr.get(), name() });
	_normalize(solutionPtr);
	auto result = run(solutionPtr);
	Ctoolhu::Event::Fire(Events::Finished { solutionPtr.get(), name() });
	return result;
}

void AlgorithmBase::stop()
{
	lock_guard _(_pauseMutex);
	_stopRequested = true;
}

bool AlgorithmBase::isStopRequested() const
{
	unique_lock lock(_pauseMutex);

	//if paused, don't allow continuation until resumed
	while (_paused && !_stopRequested)
		_pauseChanged.wait(lock);

	return _stopRequested || (_parent && _parent->isStopRequested());
}

chrono::milliseconds AlgorithmBase::elapsedTime() const
{
	return _timer.ElapsedTime();
}

void AlgorithmBase::_normalize(solution_ptr_t solutionPtr) const
{
	bool changed = solutionPtr->normalize();
	solutionPtr->getFitness(); //to make sure it's initialized
	if (changed) {
		//results of the normalization must always be treated as a better solution,
		//no matter the actual fitness
		Ctoolhu::Event::Fire(Events::BestSolutionFound { solutionPtr.get(), elapsedTime() });
		if (solutionPtr->isFeasible())
			Ctoolhu::Event::Fire(Events::FeasibleSolutionFound { solutionPtr.get(), elapsedTime() });
	}
}

void AlgorithmBaseConfig::load(const boost::property_tree::ptree &pt)
{
	keepFeasible = pt.get("keepFeasible", false);
	extended = pt.get("extended", false);
	benchmark = pt.get("benchmark", false);
}

void AlgorithmBaseConfig::propagate(boost::property_tree::ptree &dst) const
{
	dst.add("keepFeasible", keepFeasible);
	dst.add("extended", extended);
	dst.add("benchmark", benchmark);
}

} //ns Algorithm
