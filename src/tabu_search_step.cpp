//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/tabu_search_step.h>

namespace Algorithm { namespace TabuSearch {

AdaptedFitness::delta_t Step::adaptedDelta() const
{
	return _adaptedDelta;
}

void Step::adaptedDelta(AdaptedFitness::delta_t in)
{
	_adaptedDelta = in;
}

Fitness::delta_t Step::delta() const
{
	return _delta;
}

void Step::delta(Fitness::delta_t in)
{
	_delta = in;
}

} } //ns Algorithm::TabuSearch
