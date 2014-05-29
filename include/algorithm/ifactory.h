//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_ifactory_included_
#define _algorithm_ifactory_included_

#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>
#include <string>

namespace Algorithm {

	class IAlgorithm;

	//interface for classes capable of producing arbitrary algorithm objects
	class IFactory {

	  public:

		typedef std::unique_ptr<IAlgorithm> ialgorithm_ptr_type;

		//returns new instance of the algorithm registered under the given name, configured from given proptree
		virtual ialgorithm_ptr_type CreateAlgorithm(
			const std::string &name,
			const boost::property_tree::ptree &config) const = 0;

		virtual ~IFactory() {};
	};

} //ns Algorithm

#endif //file guard
