#ifndef _algorithm_istore_included_
#define _algorithm_istore_included_

#include <ctoolhu/thread/lockable.hpp>
#include <ctoolhu/thread/proxy.hpp>
#include <mutex>

namespace Algorithm {

	class ISolution;

	namespace Storage {

		//Interface for thread-safe storage for solutions
		class IStore : public Ctoolhu::Thread::Lockable<std::mutex> {

		  public:

			virtual ~IStore() = default;

			virtual ISolution *getCurrentSolution() const = 0;
			virtual ISolution *getBestSolution() const = 0;
			virtual ISolution *getFeasibleSolution() const = 0;

			virtual ISolution *getBestOverallSolution() const = 0;
			virtual ISolution *getFeasibleOverallSolution() const = 0;

			virtual void setBestSolution() = 0;
			virtual void setFeasibleSolution() = 0;
			virtual void markDirty() = 0;
		};

	} //ns Storage

} //ns Algorithm

#endif //file guard
