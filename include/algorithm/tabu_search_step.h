//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_search_step_included_
#define _algorithm_tabu_search_step_included_

#include "itabusearchstep.h"
#include "fitness.h"

namespace Algorithm {

	namespace TabuSearch {

		class Step : public IStep {

		  public:

			AdaptedFitness::delta_t adaptedDelta() const final;
			void adaptedDelta(AdaptedFitness::delta_t) final;

			Fitness::delta_t delta() const final;
			void delta(Fitness::delta_t) final;

		  protected:

			Step() = default;

		  private:

			AdaptedFitness::delta_t _adaptedDelta{0};
			Fitness::delta_t _delta{0};
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
