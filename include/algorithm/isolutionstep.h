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

		virtual AdaptedFitness::delta_type adaptedDelta() const = 0; //getter
		virtual void adaptedDelta(AdaptedFitness::delta_type) = 0; //setter

		virtual Fitness::delta_type delta() const = 0;
		virtual void delta(Fitness::delta_type) = 0;

		virtual void Execute(ISolution *) const = 0; //perform this step
		virtual void dump(std::ostream &) const = 0;

		virtual ~ISolutionStep() {};
	};

} //ns Algorithm

#endif
