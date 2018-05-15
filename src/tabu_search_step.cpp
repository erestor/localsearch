//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/tabu_search_step.h>

namespace Algorithm { namespace TabuSearch {

AdaptedFitness::delta_type Step::adaptedDelta() const
{
	return _adaptedDelta;
}

void Step::adaptedDelta(AdaptedFitness::delta_type in)
{
	_adaptedDelta = in;
}

Fitness::delta_type Step::delta() const
{
	return _delta;
}

void Step::delta(Fitness::delta_type in)
{
	_delta = in;
}

} } //ns Algorithm::TabuSearch
