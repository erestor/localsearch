//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_factory_included_
#define _algorithm_factory_included_

#include "ifactory.h"
#include <loki/singleton.h>
#include <map>

namespace Algorithm {

	class Factory : public IFactory {

		typedef std::function<ialgorithm_ptr_type(const boost::property_tree::ptree &)> maker_type;

	  public:

		//returns new algorithm registered under the name, or NullAlgorithm instance if not found.
		ialgorithm_ptr_type CreateAlgorithm(const std::string &name, const boost::property_tree::ptree &config) const final;

		//registers algorithm constructor with the factory.
		//throws runtime exception if registration fails.
		//the return value is always true, used typically to initialize a const bool global variable in a no-name namespace.
		bool Register(const std::string &name, const maker_type &);

		//registers all directly usable algorithms in this library with the factory
		//must not be called, it's here only to reference the registering functions so that they're not dropped by the linker
		static void __RegisterNative();

	  private:

		friend struct Loki::CreateUsingNew<Factory>;
		Factory() = default;

		std::map<std::string, maker_type> _registry;
	};

	typedef Loki::SingletonHolder<Factory> SingleFactory;

	//creates an algorithm using a single tree with "name" and "config" nodes determining an algorithm.
	//throws if either name or config aren't present in the definition tree.
	IFactory::ialgorithm_ptr_type Create(const boost::property_tree::ptree &definition);

	//returns the NullAlgorithm
	IFactory::ialgorithm_ptr_type GetNullAlgorithm();

} //ns Algorithm

#endif //file guard

