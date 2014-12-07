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

using namespace Ctoolhu;
using namespace std;

namespace Algorithm { namespace TabuSearch {

Searcher::Searcher(const boost::property_tree::ptree &pt)
:	
	_tabuList(pt)
{
	_config.keepFeasible = pt.get("keepFeasible", false);
	_config.maxSteps = pt.get("maxSteps", 200);
	_config.dynamicAdaptationThreshold = pt.get("dynamicAdaptationThreshold", 10);
}

//execute the algorithm
bool Searcher::Run(solution_ptr_type solutionPtr)
{
	_currentSolutionPtr = solutionPtr.get();
	_currentSolutionPtr->GetFitness();
	_bestSolutionPtr = unique_ptr<ISolution>(_currentSolutionPtr->Clone());
	Event::Fire(Events::Started { _currentSolutionPtr });
	bool improved = false;
	int noImprovements = 0;
	while (!IsStopRequested() && (int)_bestSolutionPtr->GetFitness() > 0 && (noImprovements < _config.maxSteps)) {
		noImprovements++;
		auto possibleSteps = GetBestSteps();

		//update the tabu list now so that new entries added when executing the move/swap stay intact for next step
		//also to possibly allow some steps for next move in case no steps have just been found
		_tabuList.Shift();

		step_ptr_type nextStepPtr = _GetNextStep(possibleSteps);
		if (!nextStepPtr) {
			//no possible steps at this point - might be all tabu, let's try again
			continue;
		}
		Event::Fire<Events::BeforeTabuSearchStep>();
		nextStepPtr->Execute(_currentSolutionPtr);

		//prepare the event with step data
		stringstream s;
		nextStepPtr->Dump(s);
		Events::AfterTabuSearchStep evATSS {
			_config.dynamicAdaptationThreshold,
			_currentSolutionPtr,
			s.str()
		};

		_tabuList.Insert(move(nextStepPtr));
		//nextStepPtr is no longer available

		Event::Fire(evATSS);

		//check if new best solution was found, in that case store it
		if (_currentSolutionPtr->GetFitness() < _bestSolutionPtr->GetFitness()
		&& (!_config.keepFeasible || !_bestSolutionPtr->IsFeasible() || _currentSolutionPtr->IsFeasible())) {
			noImprovements = 0;
			improved = true;
			_currentSolutionPtr->CopyTo(_bestSolutionPtr.get());
			Event::Fire(Algorithm::Events::BestSolutionFound { _currentSolutionPtr, ElapsedTime() });
		}
	}
	//cycle is finished with some solution, make sure we use the best found
	_bestSolutionPtr->CopyTo(_currentSolutionPtr);
	Event::Fire(Events::Finished { _currentSolutionPtr });
	return improved;
}

//returns random element from the _PossibleSteps container (next step the solution should take)
Searcher::step_ptr_type Searcher::_GetNextStep(vector<step_ptr_type> &steps) const
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
			Event::Fire(Events::AfterRandomStepChosen { size, vectorIndex });
		}
	}
	return move(steps[vectorIndex]);
}

const Fitness::delta_type &Searcher::GetWorstDelta() const
{
	static Fitness::delta_type worst { numeric_limits<Fitness::delta_type>::max() };
	return worst;
}

//assesses given step in the context of the running algorithm to see if it's a candidate for continuation
bool Searcher::IsAcceptableStep(const IStep *stepPtr, const Fitness &currentFitness) const
{
	return _IsAspirationStep(stepPtr, currentFitness) || !_tabuList.IsTabu(stepPtr);
}

//aspiration steps are allowed to happen even if they are in the tabu list
bool Searcher::_IsAspirationStep(const ISolutionStep *stepPtr, const Fitness &currentFitness) const
{
	return (currentFitness + stepPtr->Delta()) < _bestSolutionPtr->GetFitness();
}

} } //ns Algorithm::TabuSearch
