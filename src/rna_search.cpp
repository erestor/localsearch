//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/rna_search.h>
#include <algorithm/rna_search_events.h>
#include <algorithm/events.h>
#include <algorithm/isolution.h>
#include <ctoolhu/event/firer.hpp>
#include <boost/property_tree/ptree.hpp>
#include <stdexcept>

using namespace Ctoolhu;
using namespace std;

namespace Algorithm { namespace RNA {

Searcher::Searcher(const boost::property_tree::ptree &pt)
{
	_config.load(pt);
	_config.maxSteps = pt.get("maxSteps", 1000);
	_config.tickFrequency = max(1, pt.get("tickFrequency", 1000));
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

bool Searcher::run(solution_ptr_t currentSolutionPtr)
{
	int maxSteps = _config.maxSteps;
	if (_config.extended)
		maxSteps *= 2;

	const Fitness starting{currentSolutionPtr->getFitness()};
	Fitness bestFeasible{currentSolutionPtr->isFeasible() ? starting : Fitness::worst()}; //holds the fitness of the best feasible solution found so far
	bool improved = false;
	int noImprovements = 0;
	int steps = 0;
	while (!isStopRequested() && (int)currentSolutionPtr->getFitness() > 0 && (noImprovements < maxSteps)) {
		noImprovements++;
		steps++;
		const Fitness original{currentSolutionPtr->getFitness()};
		auto const delta = walk(currentSolutionPtr.get());
		const Fitness actual{currentSolutionPtr->getFitness()};
		if (actual != original + delta)
			throw logic_error("Algorithm::RNA::Searcher::run: unexpected fitness delta after walk. Expected " + to_string(delta) + ", got " + to_string(actual - original));

		if (delta > 0)
			throw logic_error("Algorithm::RNA::Searcher::run: search step has positive delta");

		if (delta < 0) {
			noImprovements = 0;
			improved = true;
			Event::Fire(Algorithm::Events::CurrentSolutionChanged { currentSolutionPtr.get(), elapsedTime() });
			Event::Fire(Algorithm::Events::BestSolutionFound { currentSolutionPtr.get(), elapsedTime() });
		}
		if (currentSolutionPtr->isFeasible() && actual < bestFeasible) {
			bestFeasible = actual;
			Event::Fire(Algorithm::Events::FeasibleSolutionFound { currentSolutionPtr.get(), elapsedTime() });
		}
		if (steps % _config.tickFrequency == 0)
			Event::Fire<Events::Tick>();
	}
	
	//never worse than original
	if (currentSolutionPtr->getFitness() > starting)
		throw logic_error("Algorithm::RNA::Searcher::run: search worsened the solution");

	return improved;
}

} } //ns Algorithm::RNA
