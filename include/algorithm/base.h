//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_base_included_
#define _algorithm_base_included_

#include "ialgorithm.h"
#include "fitness.h"
#include <ctoolhu/time/timer.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <condition_variable>
#include <mutex>

namespace Algorithm {

	class AlgorithmBase : public IAlgorithm {

#if __cplusplus < 201703L
		typedef std::lock_guard<std::mutex> lock_guard;
		typedef std::unique_lock<std::mutex> unique_lock;
#endif

		typedef std::chrono::milliseconds timer_resolution;

	  public:

		bool start(solution_ptr_t) final;
		void pauseAsync() override;			//thread-safe
		void resumeAsync() override;		//thread-safe
		void stopAsync() override;			//thread-safe
		bool isStopRequested() const final; //thread-safe

		void setParent(const IAlgorithm *parent) final; //for nested algorithms
		timer_resolution elapsedTime() const final;

	  protected:

		AlgorithmBase();
		AlgorithmBase(const AlgorithmBase &) = delete;

		//algorithm body, should return true if solution was improved.
		//takes solution ptr by value so that a shared_ptr is copied (if used),
		//making sure the solution is alive for the whole run of the algorithm.
		//The implementation should be in 'run' (instead of in 'start')
		virtual bool run(solution_ptr_t startingSolutionPtr) = 0;

	  private:

		void _normalize(solution_ptr_t) const;
		void _togglePause(bool pause);

		//algorithm stopping and pausing mechanism
		bool _stopRequested;
		bool _paused;
		mutable std::mutex _pauseMutex;
		mutable std::condition_variable _pauseChanged;

		const IAlgorithm *_parent;
		Ctoolhu::Time::Timer<timer_resolution> _timer;
	};

	struct AlgorithmBaseConfig {
		bool keepFeasible{false};
		bool extended{false}; //can be used to enable extra measures to maximize the searching potential, at the cost of time
		bool benchmark{false}; //let the algorithm know benchmark mode is required (e.g. to disable seeding random engine)

		void load(const boost::property_tree::ptree &);
		void propagate(boost::property_tree::ptree &dst) const;
	};

} //ns Algorithm

#endif //file guard
