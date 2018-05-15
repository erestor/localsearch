//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include "tabu_list.h"
#include <algorithm/itabusearchstep.h>
#include <ctoolhu/std_ext.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;

namespace Algorithm { namespace TabuSearch {

TabuList::TabuList(const boost::property_tree::ptree &pt)
:
	_tabuGenerator(pt.get("tabuLower", 20), pt.get("tabuUpper", 30))
{
	_configure(pt);
}

void TabuList::_configure(const boost::property_tree::ptree &pt)
{
	_config.tabuLower = pt.get("tabuLower", 20);
	_config.tabuUpper = pt.get("tabuUpper", 30);
	_config.tabuShortTerm = pt.get("tabuShortTerm", 1);
}

void TabuList::insert(const IStep::ptr_t &step)
{
	_steps.emplace_back(_tabuGenerator(), step);
}

void TabuList::shift()
{
	std_ext::erase_if(_steps, [](TabuElement &elem) {
		return --elem == 0;
	});
}

bool TabuList::isTabu(const IStep *stepPtr) const
{
	//check short-term tabu in reverse order because we're inserting at the back
	int counter = 0;
	for (int i = _steps.size() - 1; i >= 0 && counter < _config.tabuShortTerm; --i, ++counter) {
		if (_steps[i].step()->isEqualForShortTermTabu(stepPtr))
			return true;
	}
	//check normal tabu
	return std_ext::any_of(_steps, [stepPtr](auto const &elem) {
		return elem.step()->isUndoOf(stepPtr);
	});
}

} } //ns Algorithm::TabuSearch
