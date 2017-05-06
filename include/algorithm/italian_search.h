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

			explicit ItalianSearch(const boost::property_tree::ptree &);
			const std::string &Name() const final;

			void EnableExtensions() final;
			void DisableExtensions() final;

		  private:

			typedef std::pair<std::string, boost::property_tree::ptree> algorithm_def_type;

			struct Config : AlgorithmBaseConfig {
				int repeat; //indicates how many times should the algorithm be repeated from the starting point (if solution isn't found)
				int cycles; //indicates how many times the child algorithms will be run in a loop on the same solution in case of no improvement
				algorithm_def_type initial; //name of initial algorithm run once (usually some kind of a generation algorithm)
				std::vector<algorithm_def_type> algorithms;
					//chain of algorithms run in cycles until stopping condition is met
			};

			bool Run(solution_ptr_type) final;

			std::vector<std::unique_ptr<IAlgorithm>> _algorithms;
			Config _config;
		};

		namespace Private {
		
			bool Register(); //registers this algorithm with the factory

		} //ns Private

	} //ns ItalianSearch

} //ns Algorithm

#endif //file guard
