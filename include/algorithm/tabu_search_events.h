//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_search_events_included_
#define _algorithm_tabu_search_events_included_

#include <string>

namespace Algorithm {

	class ISolution;

	namespace TabuSearch {

		namespace Events {

			struct Started {
				ISolution *currentSolutionPtr;
			};

			struct Finished {
				ISolution *currentSolutionPtr;
			};

			struct BeforeTabuSearchStep {};

			//executes after a single tabu search step is performed
			struct AfterTabuSearchStep {
				int dynamicAdaptationThreshold;
				ISolution *currentSolutionPtr;
				std::string stepDescription;
				bool keepFeasible;
			};

			struct AfterRandomStepChosen {
				int size;
				int index;
			};

		} //ns Events

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
