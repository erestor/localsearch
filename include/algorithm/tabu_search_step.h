//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_search_step_included_
#define _algorithm_tabu_search_step_included_

#include "itabusearchstep.h"
#include "fitness.h"

namespace Algorithm {

	namespace TabuSearch {

		//when searching for optimal solution, we take steps towards the goal - atomic operations upon the solution
		//this is one such step
		class Step : public IStep {

		  public:

			AdaptedFitness::delta_type AdaptedDelta() const final;
			void AdaptedDelta(AdaptedFitness::delta_type) final;

			Fitness::delta_type Delta() const final;
			void Delta(Fitness::delta_type) final;

		  protected:

			Step();

		  private:

			AdaptedFitness::delta_type _adaptedDelta;
			Fitness::delta_type _delta;
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
