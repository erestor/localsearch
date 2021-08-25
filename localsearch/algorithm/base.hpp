//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_base_included_
#define _algorithm_base_included_

#include "../fitness.hpp"
#include "../events/events.h"
#include "../interface/ialgorithm_exec.h"
#include "../interface/isolution.h"
#include <ctoolhu/event/firer.hpp>
#include <ctoolhu/time/timer.hpp>
#include <condition_variable>
#include <mutex>

namespace Algorithm {

	template <class Solution>
	class AlgorithmBase : public IAlgorithmExec<Solution> {

	  public:

		using timer_resolution = std::chrono::milliseconds;

		AlgorithmBase(const AlgorithmBase &) = delete;
		AlgorithmBase(AlgorithmBase &&) = delete;
		AlgorithmBase &operator=(const AlgorithmBase &) = delete;
		AlgorithmBase &operator=(AlgorithmBase &&) = delete;

		bool start(Solution &solution) final
		{
			_stopRequested = false;
			_timer.StartClock();
			Ctoolhu::Event::Fire(Events::Started { &solution, this->name() });
			_normalize(solution);
			const bool result{_run(solution)};
			Ctoolhu::Event::Fire(Events::Finished { &solution, this->name(), this });
			return result;
		}

		void pauseAsync() final
		{
			_togglePause(true);
		}

		void resumeAsync() final
		{
			_togglePause(false);
		}

		void stopAsync() final
		{
			std::lock_guard<std::mutex> _(_pauseMutex);
			_stopRequested = true;
		}

		bool isStopRequested() const final
		{
			std::unique_lock<std::mutex> lock(_pauseMutex);

			//if paused, don't allow continuation until resumed
			while (_paused && !_stopRequested)
				_pauseChanged.wait(lock);

			return _stopRequested || (_parent && _parent->isStopRequested());
		}

		//for nested algorithms
		void setParent(IAlgorithm *parent) noexcept final
		{
			_parent = parent;
		}

		timer_resolution elapsedTime() const noexcept final
		{
			return _timer.ElapsedTime();
		}

	  protected:

		AlgorithmBase() = default;

	  private:

		//algorithm body, should return true if solution was improved.
		virtual bool _run(Solution &) = 0;

		void _togglePause(bool pause)
		{
			std::unique_lock<std::mutex> lock(_pauseMutex);
			_paused = pause;
			lock.unlock(); //manual unlocking is done before notifying... (from C++ reference)
			_pauseChanged.notify_all();
		}

		void _normalize(Solution &solution) const
		{
			const bool changed = solution.normalize();
			solution.getFitness(); //to make sure it's initialized
			if (changed) {
				//results of the normalization must always be treated as a better solution,
				//no matter the actual fitness
				Ctoolhu::Event::Fire(Events::BestSolutionFound { &solution, elapsedTime() });
				if (solution.isFeasible())
					Ctoolhu::Event::Fire(Events::FeasibleSolutionFound { &solution, elapsedTime() });
			}
		}

		//algorithm stopping and pausing mechanism
		bool _stopRequested{false};
		bool _paused{false};
		mutable std::mutex _pauseMutex;
		mutable std::condition_variable _pauseChanged;

		IAlgorithm *_parent{nullptr};
		Ctoolhu::Time::Timer<timer_resolution> _timer;
	};

} //ns Algorithm

#endif //file guard
