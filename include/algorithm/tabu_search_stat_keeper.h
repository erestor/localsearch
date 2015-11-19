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
			Events::AfterRandomStepChosen
		> {

		  public:

			void on(Events::BeforeStep *);
			void on(Events::AfterRandomStepChosen *);

		  protected:

			StatKeeper();

			int stepsExecuted;
			int multiChoiceSteps;
			int multiChoiceStepChoices;
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
