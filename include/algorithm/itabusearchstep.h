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

			virtual bool IsAtomicForTabu() const = 0;
				//this is a helper for the real case of TT moves and swaps. A swap is actually a set of two
				//moves performed simultaneously, and for tabu purposes swap is not an atomic step
				//and as such cannot be compared directly for tabu purposes with a move

			virtual bool IsUndo(const IStep *) const = 0;
				//this is for tabu list purposes, which means to ensure that a move isn't taken back for some time.
				//so when comparing the moves, we're actually looking at if the source of the comp is equal to the destination of
				//this move, and if destination of the comp is equal to the source of this comp
				//
				//in other words, this returns true if one is the undo of the other

			virtual bool IsEqualForShortTermTabu(const IStep *) const = 0;
				//comparison that will usually be true if the solution genes are the same
				//(the short term tabu is there to ensure a gene stays in one place for some time)
		};

	}
}

#endif //file guard