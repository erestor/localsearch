#include <algorithm/tabu_search_stat_keeper.h>

namespace Algorithm { namespace TabuSearch {

StatKeeper::StatKeeper()
:	stepsExecuted{0},
	multiChoiceSteps{0},
	multiChoiceStepChoices{0},
	cyclesDetected{0},
	stepsBetweenCycles{0}
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

void StatKeeper::on(Events::CycleDetected *ev)
{
	cyclesDetected++;
	stepsBetweenCycles += ev->stepsSinceLastImprovement;
}

} } //ns Algorithm::TabuSearch
