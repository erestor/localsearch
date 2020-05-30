//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_list_included
#define _algorithm_tabu_list_included

#include "tabu_element.hpp"
#include <ctoolhu/std_ext.hpp>
#include <ctoolhu/random/generator.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>

namespace Algorithm::TabuSearch {

	//list of steps that are tabu to perform (with exception of aspiration steps)
	template <class Step>
	class TabuList {

		public:

		explicit TabuList(const boost::property_tree::ptree &pt)
		:
			_tabuGenerator(pt.get("tabuLower", 20), pt.get("tabuUpper", 30))
		{
			_configure(pt);
		}

		void insert(const std::shared_ptr<Step> &step)
		{
			_elements.emplace_back(_tabuGenerator(), step);
		}

		void shift() //decrease tabu counter and remove all elements that reached the end of their time
		{
			std_ext::erase_if(_elements, [](auto &elem) {
				return --elem == 0;
			});
		}

		bool isTabu(const Step &step) const //return true if the given step is tabu according to the list
		{
			//check short-term tabu in reverse order because we're inserting at the back
			int counter = 0;
			for (int i = _elements.size() - 1; i >= 0 && counter < _config.tabuShortTerm; --i, ++counter) {
				//int is the correct type, because size_t is unsigned, it would overflow when size() is zero
				if (_elements[i].step()->isEqualForShortTermTabu(step))
					return true;
			}
			//check normal tabu
			return std_ext::any_of(_elements, [&step](auto const &elem) {
				return elem.step()->isUndoOf(step);
			});
		}

		private:

		struct Config {
			int tabuLower;
			int tabuUpper;
			int tabuShortTerm;
		};

		void _configure(const boost::property_tree::ptree &pt)
		{
			_config.tabuLower = pt.get("tabuLower", 20);
			_config.tabuUpper = pt.get("tabuUpper", 30);
			_config.tabuShortTerm = pt.get("tabuShortTerm", 1);
		}

		std::vector<TabuElement<Step>> _elements;
		Ctoolhu::Random::SmallIntGenerator _tabuGenerator; //random tabu iterations number generator
		Config _config;
	};

} //ns Algorithm::TabuSearch

#endif //file guard
