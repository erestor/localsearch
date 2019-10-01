//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_config_included_
#define _algorithm_config_included_

#include <boost/property_tree/ptree.hpp>

namespace Algorithm {

	struct Config {

	  public:

		bool keepFeasible{false};
		bool extended{false}; //can be used to enable extra measures to maximize the searching potential, at the cost of time
		bool benchmark{false}; //let the algorithm know benchmark mode is required (e.g. to disable seeding random engine)

		void load(const boost::property_tree::ptree &pt)
		{
			keepFeasible = pt.get("keepFeasible", false);
			extended = pt.get("extended", false);
			benchmark = pt.get("benchmark", false);
		}

		void propagate(boost::property_tree::ptree &dst) const
		{
			dst.add("keepFeasible", keepFeasible);
			dst.add("extended", extended);
			dst.add("benchmark", benchmark);
		}
	};

} //ns Algorithm

#endif //file guard
