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
	_config.Load(pt);
	_config.maxSteps = pt.get("maxSteps", 1000);
	_config.tickFrequency = max(1, pt.get("tickFrequency", 1000));
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

bool Searcher::Run(solution_ptr_t currentSolutionPtr)
{
	int maxSteps = _config.maxSteps;
	if (_config.extended)
		maxSteps *= 2;

	auto startingFitness = currentSolutionPtr->GetFitness();
	bool improved = false;
	int noImprovements = 0;
	int steps = 0;
	while (!IsStopRequested() && (int)currentSolutionPtr->GetFitness() > 0 && (noImprovements < maxSteps)) {
		noImprovements++;
		steps++;
		auto currentFitness = currentSolutionPtr->GetFitness();
		auto delta = Walk(currentSolutionPtr.get());
		if (currentSolutionPtr->GetFitness() != currentFitness + delta)
			throw logic_error("Algorithm::RNA::Searcher::Run: unexpected fitness delta after walk. Expected " + to_string(delta) + ", got " + to_string(currentSolutionPtr->GetFitness() - currentFitness));

		if (delta > 0)
			throw logic_error("Algorithm::RNA::Searcher::Run: search step has positive delta");

		if (delta < 0) {
			noImprovements = 0;
			improved = true;			
			Event::Fire(Algorithm::Events::BetterSolutionFound { currentSolutionPtr.get(), ElapsedTime() });
			if (currentSolutionPtr->IsFeasible())
				Event::Fire(Algorithm::Events::FeasibleSolutionFound { currentSolutionPtr.get(), ElapsedTime() });
		}
		if (steps % _config.tickFrequency == 0)
			Event::Fire<Events::Tick>();
	}
	
	//never worse than original
	if (currentSolutionPtr->GetFitness() > startingFitness)
		throw logic_error("Algorithm::RNA::Searcher::Run: search worsened the solution");

	return improved;
}

} } //ns Algorithm::RNA
