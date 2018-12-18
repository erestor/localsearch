//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/tabu_search.h>
#include <algorithm/events.h>
#include <algorithm/isolution.h>
#include <algorithm/itabusearchstep.h>
#include <algorithm/tabu_search_events.h>
#include <ctoolhu/event/firer.hpp>
#include <boost/property_tree/ptree.hpp>
#include <sstream>
#include <stdexcept>

using namespace Ctoolhu;
using namespace std;

namespace Algorithm { namespace TabuSearch {

Searcher::Searcher(const boost::property_tree::ptree &pt)
:
	_bestFeasibleFitness{Fitness::worst()},
	_tabuList(pt)
{
	_config.load(pt);
	_config.maxSteps = pt.get("maxSteps", 500);
	_config.dynamicAdaptationThreshold = pt.get("dynamicAdaptationThreshold", 10);
	_config.neighborhood = pt.get<string>("neighborhood", "");
}

void Searcher::enableExtensions()
{
	_config.extended = true;
}

void Searcher::disableExtensions()
{
	_config.extended = false;
}

const Searcher::Config &Searcher::getConfig() const
{
	return _config;
}

//assesses given step in the context of the running algorithm to see if it's a candidate for continuation
bool Searcher::isAcceptableStep(const IStep *stepPtr, Fitness currentFitness) const
{
	return _isAspirationStep(stepPtr, currentFitness) || !_tabuList.isTabu(stepPtr);
}

//execute the algorithm
bool Searcher::run(solution_ptr_t solutionPtr)
{
	int maxSteps = _config.maxSteps;
	if (_config.extended)
		maxSteps *= 2;

	auto currentSolutionPtr = solutionPtr.get();
	_bestSolutionPtr = unique_ptr<ISolution>(currentSolutionPtr->clone());

	Event::Fire(Events::BeforeStart{currentSolutionPtr});

	bool improved = false;
	int noImprovements = 0;
	while (!isStopRequested() && (int)_bestSolutionPtr->getFitness() > 0 && (noImprovements < maxSteps)) {
		noImprovements++;
		auto possibleSteps = getBestSteps(currentSolutionPtr);

		//update the tabu list now so that new entries added when executing the step stay intact for next step
		//also to possibly allow some steps for next move in case no steps have just been found
		_tabuList.shift();

		auto nextStepPtr = _getNextStep(possibleSteps);
		if (nextStepPtr) {
			//can be null if there are no possible steps at this point - might be all tabu
			Event::Fire(Events::BeforeStep{currentSolutionPtr});

			//record step data before execution
			stringstream s;
			nextStepPtr->dump(s);

			auto expectedFitness = currentSolutionPtr->getFitness() + nextStepPtr->delta();
			nextStepPtr->Execute(currentSolutionPtr);
			auto fitness = currentSolutionPtr->getFitness();
			if (fitness != expectedFitness)
				throw logic_error("Algorithm::TabuSearch::Searcher::run: Unexpected fitness after step execution");

			_tabuList.insert(nextStepPtr);

			Event::Fire(Events::StepExecuted {
				_config.dynamicAdaptationThreshold,
				currentSolutionPtr,
				s.str(),
				_config.keepFeasible
			});

			Event::Fire(Algorithm::Events::CurrentSolutionChanged { currentSolutionPtr, elapsedTime() });

			bool foundBest{false};
			if (fitness == _bestSolutionPtr->getFitness()) {
				//check for cycling
				if (currentSolutionPtr->isEqual(_bestSolutionPtr.get())) //only compare structure if the fitness is the same, comparison can be computationally expensive
					Event::Fire(Events::CycleDetected { noImprovements });

				if (noImprovements == maxSteps && _retainsFeasibility(currentSolutionPtr)) {
					//this is the last step, accept current solution as the best to improve success chances of chained algorithm,
					//which will continue from this solution as opposed to try with the original solution again
					foundBest = true;
				}
			}
			//check if new best solution was found, in that case store it
			else if (fitness < _bestSolutionPtr->getFitness() && _retainsFeasibility(currentSolutionPtr)) {
				noImprovements = 0;
				improved = true;
				foundBest = true;
			}
			if (foundBest) {
				currentSolutionPtr->copyTo(_bestSolutionPtr.get());
				Event::Fire(Algorithm::Events::BestSolutionFound { currentSolutionPtr, elapsedTime() });
			}
			if (currentSolutionPtr->isFeasible() && fitness < _bestFeasibleFitness) {
				_bestFeasibleFitness = fitness;
				Event::Fire(Algorithm::Events::FeasibleSolutionFound { currentSolutionPtr, elapsedTime() });
			}
		}
		Event::Fire(Events::AfterStep { noImprovements });
	}

	//Cycle is finished with some solution, make sure we use one with the best fitness found, preferring current to the saved best.
	//The reason is that next algorithm in the chain can have a chance to work on a different solution than in the previous cycle.
	if (currentSolutionPtr->getFitness() > _bestSolutionPtr->getFitness())
		_bestSolutionPtr->copyTo(currentSolutionPtr);

	Event::Fire(Events::Finished { currentSolutionPtr });
	return improved;
}

//returns random element from the _PossibleSteps container (next step the solution should take)
IStep::ptr_t Searcher::_getNextStep(const vector<IStep::ptr_t> &steps) const
{
	int vectorIndex = 0;
	size_t size = steps.size();
	switch (size) {
		case 0:
			//no valid steps found, can happen for extremely short timetables when all moves are tabu e.g.
			return nullptr;
		case 1:
			break;
		default: {
			Random::IntGenerator RandStep(0, size - 1);
			vectorIndex = RandStep();
			Event::Fire(Events::AfterRandomStepChosen { static_cast<int>(size), vectorIndex });
		}
	}
	return steps[vectorIndex];
}

//aspiration steps are allowed to happen even if they are in the tabu list
bool Searcher::_isAspirationStep(const ISolutionStep *stepPtr, Fitness currentFitness) const
{
	return (currentFitness + stepPtr->delta()) < _bestSolutionPtr->getFitness();
}

bool Searcher::_retainsFeasibility(ISolution *solutionPtr) const
{
	return !_config.keepFeasible || !_bestSolutionPtr->isFeasible() || solutionPtr->isFeasible();
}

} } //ns Algorithm::TabuSearch
