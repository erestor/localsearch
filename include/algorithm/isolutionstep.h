//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_isolutionstep_included_
#define _algorithm_isolutionstep_included_

#include "fitness.h"

namespace Algorithm {

	class ISolution;

	//step is a general operation which modifies a solution
	class ISolutionStep {

	  public:

		virtual AdaptedFitness::delta_t adaptedDelta() const = 0; //getter
		virtual void adaptedDelta(AdaptedFitness::delta_t) = 0; //setter

		virtual Fitness::delta_t delta() const = 0;
		virtual void delta(Fitness::delta_t) = 0;

		virtual void Execute(ISolution *) const = 0; //perform this step
		virtual void dump(std::ostream &) const = 0;

		virtual ~ISolutionStep() = default;
	};

} //ns Algorithm

#endif
