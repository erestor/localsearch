//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_element_included
#define _algorithm_tabu_element_included

#include <algorithm/itabusearchstep.h>
#include <boost/config.hpp>

namespace Algorithm {

	namespace TabuSearch {

		//member of the tabu list
		class TabuElement {

		  public:

			TabuElement(int lifetime, const IStep::ptr_t &) noexcept;

			int operator --() noexcept; //prefix decrement
			const IStep::ptr_t &Step() const;

		  private:

			int _lifetime; //how long will the element stay in the list
			IStep::ptr_t _step; //step data so that we can compare with another step to determine if it's tabu
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
