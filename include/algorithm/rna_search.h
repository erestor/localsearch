//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_rna_search_included_
#define _algorithm_rna_search_included_

#include "base.h"
#include "fitness.h"
#include <boost/property_tree/ptree_fwd.hpp>

namespace Algorithm {

	namespace RNA {

		//Random Non-Ascendent fitness improvement algorithm
		class Searcher : public AlgorithmBase {

		  public:

			struct Config : AlgorithmBaseConfig {
				int maxSteps;
				int tickFrequency; //how many steps between ticks
			};

			void EnableExtensions() final;
			void DisableExtensions() final;

		  protected:

			Searcher(const boost::property_tree::ptree &config);
			bool Run(solution_ptr_type) override;
			virtual Fitness::delta_type Walk() = 0;

			ISolution *_currentSolutionPtr;
			Config _config;
		};

	} //ns RNA

} //ns Algorithm

#endif //_rna_included_
