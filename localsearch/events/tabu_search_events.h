//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_search_events_included_
#define _algorithm_tabu_search_events_included_

#include <chrono>
#include <string>

namespace Algorithm {

	class ISolution;

	namespace TabuSearch {

		namespace Events {

			struct BeforeStart {
				ISolution *currentSolutionPtr;
			};

			struct Finished {
				ISolution *currentSolutionPtr;
				int executedSteps;
				std::chrono::milliseconds elapsedTime;
			};

			struct BeforeStep {
				ISolution *currentSolutionPtr;
			};

			//executes after a single tabu search step is performed
			struct StepExecuted {
				int dynamicAdaptationThreshold;
				ISolution *currentSolutionPtr;
				std::string stepDescription;
				bool keepFeasible;
			};

			struct AfterStep {
				int noImprovements;
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
