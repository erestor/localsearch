//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_list_included
#define _algorithm_tabu_list_included

#include "tabu_element.h"
#include <ctoolhu/random/generator.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <vector>

namespace Algorithm {

	namespace TabuSearch {

		class IStep;

		//list of steps that are tabu to perform (with exception of aspiration steps)
		class TabuList {

		  public:

			explicit TabuList(const boost::property_tree::ptree &);

			void Insert(const IStep::ptr_t &);
			void Shift(); //decrease tabu counter and remove all elements that reached the end of their time

			bool IsTabu(const IStep *) const; //return true if the given step is tabu according to the list

		  private:

			struct Config {
				int tabuLower;
				int tabuUpper;
				int tabuShortTerm;
			};

			void _Configure(const boost::property_tree::ptree &);

			std::vector<TabuElement> _steps;
			Ctoolhu::Random::SmallIntGenerator _tabuGenerator; //random tabu iterations number generator
			Config _config;
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
