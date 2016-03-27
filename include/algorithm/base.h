//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_base_included_
#define _algorithm_base_included_

#include "ialgorithm.h"
#include "fitness.h"
#include <ctoolhu/time/timer.hpp>
#include <condition_variable>
#include <mutex>

namespace Algorithm {

	class AlgorithmBase : public IAlgorithm {

		typedef std::lock_guard<std::mutex> lock_guard;
		typedef std::unique_lock<std::mutex> unique_lock;
		typedef std::chrono::milliseconds timer_resolution;

	  public:

		//algorithm body, should return true if solution was improved.
		//takes solution ptr by value so that a shared_ptr is copied (if used),
		//making sure the solution is alive for the whole run of the algorithm
		virtual bool Run(solution_ptr_type startingSolutionPtr) = 0;

		void SetParent(const IAlgorithm *parent) final; //for nested algorithms
		bool Start(const solution_ptr_type &) final;
		void Stop() override;		//thread-safe
		void Pause() override;		//thread-safe
		void Resume() override;		//thread-safe
		bool IsStopRequested() const override; //thread-safe
		timer_resolution ElapsedTime() const override;

		const Fitness &GetWorstFitness() const;

	  protected:

		AlgorithmBase();

	  private:

		void _Normalize(const solution_ptr_type &) const;
		void _TogglePause(bool pause);

		//algorithm stopping and pausing mechanism
		bool _stopRequested;
		bool _paused;
		mutable std::mutex _pauseMutex;
		mutable std::condition_variable _pauseChanged;

		const IAlgorithm *_parent;
		Ctoolhu::Time::Timer<timer_resolution> _timer;
	};

	struct AlgorithmBaseConfig {
		bool keepFeasible;
		bool extended; //can be used to enable extra measures to maximize the searching potential, at the cost of time
	};

} //ns Algorithm

#endif //file guard
