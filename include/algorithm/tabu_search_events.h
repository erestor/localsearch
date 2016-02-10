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

			struct Finished {
				ISolution *currentSolutionPtr;
			};

			struct BeforeStep {};

			//executes after a single tabu search step is performed
			struct AfterStep {
				int dynamicAdaptationThreshold;
				ISolution *currentSolutionPtr;
				std::string stepDescription;
				bool keepFeasible;
			};

			struct AfterRandomStepChosen {
				int size;
				int index;
			};

			struct CycleDetected {
				int stepsSinceLastImprovement;
			};

		} //ns Events

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
