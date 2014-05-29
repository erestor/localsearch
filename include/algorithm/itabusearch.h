//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_itabusearch_included_
#define _algorithm_itabusearch_included_

#include "fitness.h"

namespace Algorithm {

	namespace TabuSearch {

		class IStep;

		//general interface for tabu search algorithm necessary for finding best next steps within a solution
		class ISearcher {

		  public:

			//worst value of step delta (never expected to be reached by an actual step)
			virtual const Fitness::delta_type &GetWorstDelta() const = 0;

			//returns true if the step can be considered as the next one to take
			//the current fitness is passed in because the algorithm might be in the middle of tweaking the current solution,
			//which is therefore not safe to be accessed
			virtual bool IsAcceptableStep(const IStep *, const Fitness &currentFitness) const = 0;
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
