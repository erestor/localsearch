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

			explicit Searcher(const boost::property_tree::ptree &config);

			struct Config : AlgorithmBaseConfig {
				int maxSteps;
				int tickFrequency; //how many steps between ticks
			};

			void enableExtensions() final;
			void disableExtensions() final;

			const Config &getConfig() const;

		  protected:

			bool run(solution_ptr_t) override;
			virtual Fitness::delta_type walk(ISolution *) = 0;

		  private:

			Config _config;
		};

	} //ns RNA

} //ns Algorithm

#endif //_rna_included_
