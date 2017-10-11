//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include "tabu_element.h"
#include <algorithm/itabusearchstep.h>

namespace Algorithm { namespace TabuSearch {

TabuElement::TabuElement(int lifetime, const step_ptr_type &step) noexcept
:
	_lifetime{lifetime},
	_step{step}
{
}

int TabuElement::operator --() noexcept
{
	return --_lifetime;
}

const TabuElement::step_ptr_type &TabuElement::Step() const
{
	return _step;
}

} } //ns Algorithm::TabuSearch
