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

			AdaptedFitness::delta_type adaptedDelta() const final;
			void adaptedDelta(AdaptedFitness::delta_type) final;

			Fitness::delta_type delta() const final;
			void delta(Fitness::delta_type) final;

		  protected:

			Step() = default;

		  private:

			AdaptedFitness::delta_type _adaptedDelta{0};
			Fitness::delta_type _delta{0};
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
