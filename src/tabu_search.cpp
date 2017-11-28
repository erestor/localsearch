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
	_tabuList(pt)
{
	_config.Load(pt);
	_config.maxSteps = pt.get("maxSteps", 500);
	_config.dynamicAdaptationThreshold = pt.get("dynamicAdaptationThreshold", 10);
	_config.neighborhood = pt.get<string>("neighborhood", "");
}

void Searcher::EnableExtensions()
{
	_config.extended = true;
}

void Searcher::DisableExtensions()
{
	_config.extended = false;
}

const Searcher::Config &Searcher::GetConfig() const
{
	return _config;
}

Fitness::delta_type Searcher::GetWorstDelta()
{
	return numeric_limits<Fitness::delta_type>::max();
}

//assesses given step in the context of the running algorithm to see if it's a candidate for continuation
bool Searcher::IsAcceptableStep(const IStep *stepPtr, Fitness currentFitness) const
{
	return _IsAspirationStep(stepPtr, currentFitness) || !_tabuList.IsTabu(stepPtr);
}

//execute the algorithm
bool Searcher::Run(solution_ptr_t solutionPtr)
{
	int maxSteps = _config.maxSteps;
	if (_config.extended)
		maxSteps *= 2;

	auto currentSolutionPtr = solutionPtr.get();
	_bestSolutionPtr = unique_ptr<ISolution>(currentSolutionPtr->Clone());

	Event::Fire(Events::BeforeStart{currentSolutionPtr});

	bool improved = false;
	int noImprovements = 0;
	while (!IsStopRequested() && (int)_bestSolutionPtr->GetFitness() > 0 && (noImprovements < maxSteps)) {
		noImprovements++;
		auto possibleSteps = GetBestSteps(currentSolutionPtr);

		//update the tabu list now so that new entries added when executing the step stay intact for next step
		//also to possibly allow some steps for next move in case no steps have just been found
		_tabuList.Shift();

		auto nextStepPtr = _GetNextStep(possibleSteps);
		if (nextStepPtr) {
			//can be null if there are no possible steps at this point - might be all tabu
			Event::Fire(Events::BeforeStep{currentSolutionPtr});

			//record step data before execution
			stringstream s;
			nextStepPtr->Dump(s);

			auto expectedFitness = currentSolutionPtr->GetFitness() + nextStepPtr->Delta();
			nextStepPtr->Execute(currentSolutionPtr);
			if (currentSolutionPtr->GetFitness() != expectedFitness)
				throw logic_error("Algorithm::TabuSearch::Searcher::Run: Unexpected fitness after step execution");

			_tabuList.Insert(nextStepPtr);

			Event::Fire(Events::StepExecuted {
				_config.dynamicAdaptationThreshold,
				currentSolutionPtr,
				s.str(),
				_config.keepFeasible
			});

			if (currentSolutionPtr->GetFitness() == _bestSolutionPtr->GetFitness()) {
				//check for cycling
				if (currentSolutionPtr->IsEqual(_bestSolutionPtr.get())) //only compare structure if the fitness is the same, comparison can be computationally expensive
					Event::Fire(Events::CycleDetected { noImprovements });

				if (noImprovements == maxSteps && _RetainsFeasibility(currentSolutionPtr)) {
					//this is the last step, accept current solution as the best to improve success chances of chained algorithm,
					//which will continue from this solution as opposed to try with the original solution again
					currentSolutionPtr->CopyTo(_bestSolutionPtr.get());
					Event::Fire(Algorithm::Events::BestSolutionFound { currentSolutionPtr, ElapsedTime() });
				}
			}
			//check if new best solution was found, in that case store it
			else if (currentSolutionPtr->GetFitness() < _bestSolutionPtr->GetFitness() && _RetainsFeasibility(currentSolutionPtr)) {
				noImprovements = 0;
				improved = true;
				currentSolutionPtr->CopyTo(_bestSolutionPtr.get());
				Event::Fire(Algorithm::Events::BestSolutionFound { currentSolutionPtr, ElapsedTime() });
			}

			if (currentSolutionPtr->IsFeasible()) {
				if (!_feasibleSolutionPtr) {
					_feasibleSolutionPtr = unique_ptr<ISolution>(currentSolutionPtr->Clone());
					Event::Fire(Algorithm::Events::FeasibleSolutionFound { currentSolutionPtr, ElapsedTime() });
				}
				else if (currentSolutionPtr->GetFitness() < _feasibleSolutionPtr->GetFitness()) {
					currentSolutionPtr->CopyTo(_feasibleSolutionPtr.get());
					Event::Fire(Algorithm::Events::FeasibleSolutionFound { currentSolutionPtr, ElapsedTime() });
				}
			}
		}
		Event::Fire(Events::AfterStep { noImprovements });
	}

	//Cycle is finished with some solution, make sure we use one with the best fitness found, preferring current to the saved best.
	//The reason is that next algorithm in the chain can have a chance to work on a different solution than in the previous cycle.
	if (currentSolutionPtr->GetFitness() > _bestSolutionPtr->GetFitness())
		_bestSolutionPtr->CopyTo(currentSolutionPtr);

	Event::Fire(Events::Finished { currentSolutionPtr });
	return improved;
}

//returns random element from the _PossibleSteps container (next step the solution should take)
IStep::ptr_t Searcher::_GetNextStep(const vector<IStep::ptr_t> &steps) const
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
bool Searcher::_IsAspirationStep(const ISolutionStep *stepPtr, Fitness currentFitness) const
{
	return (currentFitness + stepPtr->Delta()) < _bestSolutionPtr->GetFitness();
}

bool Searcher::_RetainsFeasibility(ISolution *solutionPtr) const
{
	return !_config.keepFeasible || !_bestSolutionPtr->IsFeasible() || solutionPtr->IsFeasible();
}

} } //ns Algorithm::TabuSearch
