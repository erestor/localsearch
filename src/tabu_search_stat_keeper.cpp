#include <algorithm/tabu_search_stat_keeper.h>

namespace Algorithm { namespace TabuSearch {

StatKeeper::StatKeeper()
:	stepsExecuted{0},
	multiChoiceSteps{0},
	multiChoiceStepChoices{0}
{
}

void StatKeeper::on(Events::BeforeStep *)
{
	stepsExecuted++;
}

void StatKeeper::on(Events::AfterRandomStepChosen *ev)
{
	multiChoiceSteps++;
	multiChoiceStepChoices += ev->size;
}

} } //ns Algorithm::TabuSearch
