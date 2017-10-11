//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_element_included
#define _algorithm_tabu_element_included

#include <algorithm/itabusearchstep.h>
#include <boost/config.hpp>
#include <memory>

namespace Algorithm {

	namespace TabuSearch {

		//member of the tabu list
		class TabuElement {

		  public:

			typedef std::shared_ptr<IStep> step_ptr_type;

			TabuElement(int lifetime, const step_ptr_type &) noexcept;

			int operator --() noexcept; //prefix decrement
			const step_ptr_type &Step() const;

		  private:

			int _lifetime; //how long will the element stay in the list
			step_ptr_type _step; //step data so that we can compare with another step to determine if it's tabu
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
