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

		struct Started {
			ISolution *solutionPtr;
			std::string algorithmName;
		};

		struct Finished {
			ISolution *solutionPtr;
			std::string algorithmName;
		};

		struct CurrentSolutionChanged {
			ISolution *solutionPtr;
			std::chrono::milliseconds elapsedTime;
		};

		struct BestSolutionFound {
			ISolution *solutionPtr;
			std::chrono::milliseconds elapsedTime;
		};

		struct FeasibleSolutionFound {
			ISolution *solutionPtr;
			std::chrono::milliseconds elapsedTime;
		};

		struct ExtensionsEnabled {
		};

	} //ns Events

} //ns Algorithm

#endif
