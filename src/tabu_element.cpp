//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include "tabu_element.h"
#include <algorithm/itabusearchstep.h>

namespace Algorithm { namespace TabuSearch {

TabuElement::TabuElement(int lifetime, step_ptr_type &&step) noexcept
:
	_lifetime(lifetime),
	_step(move(step))
{
}

TabuElement::TabuElement(TabuElement &&src) noexcept
:
	_lifetime(src._lifetime),
	_step(move(src._step))
{
}

TabuElement &TabuElement::operator =(TabuElement &&src) noexcept
{
	if (this != &src) {
		_step = move(src._step);
		_lifetime = src._lifetime;
	}
	return *this;
}

int TabuElement::operator --()
{
	return --_lifetime;
}

const TabuElement::step_ptr_type &TabuElement::Step() const
{
	return _step;
}

} } //ns Algorithm::TabuSearch
