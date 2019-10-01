//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_istep_included_
#define _algorithm_istep_included_

#include "../fitness.hpp"
#include <iosfwd>

namespace Algorithm {

	//derive from this to create concrete steps used e.g. in tabu search
	template <class Solution>
	class IStep {

	  public:

		virtual ~IStep() = default;

		virtual AdaptedFitness::delta_t adaptedDelta() const = 0;

		virtual Fitness::delta_t delta() const = 0;

		//perform this step
		virtual void execute(Solution &) const = 0;

		virtual void dump(std::ostream &) const = 0;
	};

} //ns Algorithm

#endif //file guard
