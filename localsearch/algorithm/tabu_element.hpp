#ifndef _algorithm_tabu_element_included
#define _algorithm_tabu_element_included

#include <memory>

namespace Algorithm::TabuSearch {

	//member of the tabu list
	template <class Step>
	class TabuElement {

	  public:

		TabuElement(int lifetime, const std::shared_ptr<Step> &step) noexcept
		:
			_lifetime{lifetime},
			_step{step}
		{
		}

		int operator--() noexcept //prefix decrement
		{
			return --_lifetime;
		}

		const std::shared_ptr<Step> &step() const noexcept { return _step; }

	  private:

		int _lifetime; //how long will the element stay in the list
		std::shared_ptr<Step> _step; //step data so that we can compare with another step to determine if it's tabu
	};

} //ns Algorithm::TabuSearch

#endif //file guard
