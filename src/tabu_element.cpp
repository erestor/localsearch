//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include "tabu_element.h"
#include <algorithm/itabusearchstep.h>

namespace Algorithm { namespace TabuSearch {

TabuElement::TabuElement(int lifetime, const IStep::ptr_t &step) noexcept
:
	_lifetime{lifetime},
	_step{step}
{
}

int TabuElement::operator --() noexcept
{
	return --_lifetime;
}

const IStep::ptr_t &TabuElement::step() const
{
	return _step;
}

} } //ns Algorithm::TabuSearch
