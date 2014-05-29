//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/tabu_search_step.h>

namespace Algorithm { namespace TabuSearch {

Step::Step()
:
	_adaptedDelta(0),
	_delta(0)
{
}

AdaptedFitness::delta_type Step::AdaptedDelta() const
{
	return _adaptedDelta;
}

void Step::AdaptedDelta(AdaptedFitness::delta_type in)
{
	_adaptedDelta = in;
}

Fitness::delta_type Step::Delta() const
{
	return _delta;
}

void Step::Delta(Fitness::delta_type in)
{
	_delta = in;
}

} } //ns Algorithm::TabuSearch
