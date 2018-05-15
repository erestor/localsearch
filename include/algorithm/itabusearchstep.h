//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_itabusearchstep_included_
#define _algorithm_itabusearchstep_included_

#include "isolutionstep.h"
#include <memory>

namespace Algorithm {

	namespace TabuSearch {

		//extension of general solution step for tabu search algorithms
		class IStep : public ISolutionStep {

		  public:

			typedef std::shared_ptr<IStep> ptr_t;
				//The steps are stored using shared pointers by default, which corresponds to the semantic of storing them in tabu list.
				//We cannot possibly assume in general case that the tabu list is the exclusive storage for a step, hence the choice of shared_ptr.

			virtual bool isUndoOf(const IStep *) const = 0;
				//this is for tabu list purposes, which means to ensure that a move isn't taken back for some time

			virtual bool isEqualForShortTermTabu(const IStep *) const = 0;
				//this is for short-term tabu list purposes
		};
	}
}

#endif //file guard