//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/base.h>
#include <boost/property_tree/ptree.hpp>

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

			struct Config {
				int cycles;
				std::string generationName;
				std::string roomRNAName;
				std::string roomTabuSearchName;
				std::string timeRNAName;
				std::string timeTabuSearchName;
			};

			bool Run(solution_ptr_type) final;
			void _FireBestSolutionFound();

			boost::property_tree::ptree _configPt; //stored configuration passed in at construction
			Config _config;
		};

	} //ns ItalianSearch

} //ns Algorithm