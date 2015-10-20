//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_itabusearchstep_included_
#define _algorithm_itabusearchstep_included_

#include "isolutionstep.h"

namespace Algorithm {

	namespace TabuSearch {

		//extension of general solution step for tabu search algorithms
		class IStep : public ISolutionStep {

		  public:

			virtual bool IsUndoOf(const IStep *) const = 0;
				//this is for tabu list purposes, which means to ensure that a move isn't taken back for some time

			virtual bool IsEqualForShortTermTabu(const IStep *) const = 0;
				//this is for short-term tabu list purposes
		};

	}
}

#endif //file guard