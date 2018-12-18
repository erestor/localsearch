//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_events_included_
#define _algorithm_events_included_

#include <chrono>
#include <string>

namespace Algorithm {

	class ISolution;

	namespace Events {

		namespace Private {

			struct SolutionInfo {
				ISolution *solutionPtr;
				std::chrono::milliseconds elapsedTime;
			};

		} //ns Private

		struct Started {
			ISolution *solutionPtr;
			std::string algorithmName;
		};

		struct Finished {
			ISolution *solutionPtr;
			std::string algorithmName;
		};

		struct CurrentSolutionChanged : Private::SolutionInfo {};
		struct BestSolutionFound : Private::SolutionInfo {};
		struct FeasibleSolutionFound : Private::SolutionInfo {};

		struct ExtensionsEnabled {
		};

	} //ns Events

} //ns Algorithm

#endif
