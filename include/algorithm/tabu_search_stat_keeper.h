//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_search_stat_keeper_included_
#define _algorithm_tabu_search_stat_keeper_included_

#include "tabu_search_events.h"
#include <ctoolhu/event/subscriber.hpp>

namespace Algorithm {

	namespace TabuSearch {

		class StatKeeper : Ctoolhu::Event::Subscriber<
			Events::BeforeStep,
			Events::AfterRandomStepChosen,
			Events::CycleDetected
		> {

		  public:

			void on(Events::BeforeStep *) final;
			void on(Events::AfterRandomStepChosen *) final;
			void on(Events::CycleDetected *) final;

		  protected:

			StatKeeper();

			int stepsExecuted;
			int multiChoiceSteps;
			int multiChoiceStepChoices;
			int cyclesDetected;
			int stepsBetweenCycles;
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
