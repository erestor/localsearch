//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_italian_search_included_
#define _algorithm_italian_search_included_

#include <algorithm/base.h>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <utility>
#include <vector>

namespace Algorithm {

	namespace ItalianSearch {

		//variation on local search technique introduced by Andrea Schaerf
		//This is a final implementation to be used directly - 
		//- use config to inject RNA and Tabu search algorithms and their configurations as necessary
		class ItalianSearch : public AlgorithmBase {

		  public:

			ItalianSearch(const boost::property_tree::ptree &);
			const std::string &Name() const final;

		  private:

			typedef std::pair<std::string, boost::property_tree::ptree> algorithm_def_t;

			struct Config {
				int cycles;
				algorithm_def_t initial; //name of initial algorithm run once
				std::vector<algorithm_def_t> algorithms;
					//chain of algorithms run in cycles until stopping condition is met
			};

			bool Run(solution_ptr_type) final;

			Config _config;
		};

		namespace Private {
		
			bool Register(); //registers this algorithm with the factory

		} //ns Private

	} //ns ItalianSearch

} //ns Algorithm

#endif //file guard
