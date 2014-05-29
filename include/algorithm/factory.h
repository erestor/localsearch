//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include "ifactory.h"
#include <loki/singleton.h>
#include <map>

namespace Algorithm {

	class Factory : public IFactory {

		typedef std::function<ialgorithm_ptr_type(const boost::property_tree::ptree &)> maker_type;

	  public:

		//returns new algorithm registered under the name, or NullAlgorithm instance if not found.
		ialgorithm_ptr_type CreateAlgorithm(const std::string &name, const boost::property_tree::ptree &config) const final;

		//registers algorithm constructor with the factory
		bool Register(const std::string &name, const maker_type &);

	  private:

		friend struct Loki::CreateUsingNew<Factory>;
		Factory() = default;

		std::map<std::string, maker_type> _registry;
	};

	typedef Loki::SingletonHolder<Factory> SingleFactory;

} //ns Algorithm
