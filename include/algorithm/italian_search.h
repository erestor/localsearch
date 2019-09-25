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
			const std::string &name() const final;

			void enableExtensions() final;
			void disableExtensions() final;

		  private:

			using algorithm_def_t = std::pair<std::string, boost::property_tree::ptree>;

			struct Config : AlgorithmBaseConfig {
				int repeat; //indicates how many times should the algorithm be repeated from the starting point (if solution isn't found)
				int cycles; //indicates how many times the child algorithms will be run in a loop on the same solution in case of no improvement
				algorithm_def_t initial; //name of initial algorithm run once (usually some kind of a generation algorithm)
				std::vector<algorithm_def_t> algorithms;
					//chain of algorithms run in cycles until stopping condition is met
			};

			bool run(solution_ptr_t) final;

			std::vector<std::unique_ptr<IAlgorithm>> _algorithms;
			Config _config;
		};

		namespace Private {
		
			bool _register(); //registers this algorithm with the factory

		} //ns Private

	} //ns ItalianSearch

} //ns Algorithm

#endif //file guard
