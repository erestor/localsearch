#ifndef _algorithm_store_included_
#define _algorithm_store_included_

#include "interface/istore.h"
#include <memory>

namespace Algorithm::Storage {

	//Thread-safe storage for solutions
	template <class Solution>
	class Store : public IStore {

		public:

		using locked_t = Ctoolhu::Thread::LockingProxy<Store<Solution>>;

		//point current solution to given solution, also init best solution
		explicit Store(std::shared_ptr<Solution> solution)
		:	_currentSolution{solution},
			_bestSolution{std::make_unique<Solution>(*solution)},
			_bestOverallSolution{std::make_unique<Solution>(*solution)}
		{
			//Do not attempt to set feasible solution here.
			//When store is constructed, constraints might not yet be available so the feasibility assessment might not be accurate.
			//It's up to the caller to handle feasibility later
		}

		Solution *getCurrentSolution() const noexcept final { return _currentSolution.get(); }
		Solution *getBestSolution() const noexcept final { return _bestSolution.get(); }
		Solution *getFeasibleSolution() const noexcept final { return _feasibleSolution.get(); }

		//this is purely for snapshot purposes, it is not used by the algorithms
		Solution *getBestOverallSolution() const noexcept final { return _bestOverallSolution.get(); }
		Solution *getFeasibleOverallSolution() const noexcept final { return _feasibleOverallSolution.get(); }

		//hard-copy current solution to the best solution
		void setBestSolution() final
		{
			*_bestSolution = *_currentSolution;
			if (_bestSolution->getFitness() < _bestOverallSolution->getFitness())
				*_bestOverallSolution = *_bestSolution;
		}

		//hard-copy current solution to feasible solution
		void setFeasibleSolution() final
		{
			if (!_feasibleSolution)
				_feasibleSolution = std::make_unique<Solution>(*_currentSolution);
			else
				*_feasibleSolution = *_currentSolution;

			if (!_feasibleOverallSolution)
				_feasibleOverallSolution = std::make_unique<Solution>(*_feasibleSolution);
			else if (!_feasibleOverallSolution->isFeasible() || _feasibleSolution->getFitness() < _feasibleOverallSolution->getFitness()) {
				//feasible overall might not be feasible due to constraint settings change - unlike the feasibleSolution it is never reset
				*_feasibleOverallSolution = *_feasibleSolution;
			}
		}

		//invalidate fitness and feasibility status of all stored solutions
		void markDirty() final
		{
			_currentSolution->markDirty();
			_bestSolution->markDirty();
			_bestOverallSolution->markDirty();
			if (_feasibleSolution) {
				_feasibleSolution->markDirty();
				if (!_feasibleSolution->isFeasible())
					_feasibleSolution.reset();
			}
			if (_feasibleOverallSolution)
				_feasibleOverallSolution->markDirty();
		}

	  private:

		const std::shared_ptr<Solution> _currentSolution;	//solution currently worked upon by the algorithm
		const std::unique_ptr<Solution> _bestSolution;		//best solution found by the algorithm
		std::unique_ptr<Solution> _feasibleSolution;		//best feasible solution found by the algorithm

		const std::unique_ptr<Solution> _bestOverallSolution;//best overall solution found during the lifetime of the store (can be dirty)
		std::unique_ptr<Solution> _feasibleOverallSolution;	//best overall feasible solution found during the lifetime of the store (can be dirty)
	};

	//returns a locked store (given as parameter) that can be used thread-safely
	template <class Solution>
	auto lockStore(Store<Solution> &store)
	{
		return typename Store<Solution>::locked_t(&store);
	}

} //ns Algorithm::Storage

#endif //file guard
