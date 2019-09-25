//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_factory_included_
#define _algorithm_factory_included_

#include "ifactory.h"
#include <ctoolhu/singleton/holder.hpp>
#include <loki/Singleton.h>
#include <map>

namespace Algorithm {

	class Factory : public IFactory {

		using maker_t = std::function<algorithm_ptr_t(const boost::property_tree::ptree &)>;

	  public:

		//returns new algorithm registered under the name, or NullAlgorithm instance if not found.
		algorithm_ptr_t createAlgorithm(const std::string &name, const boost::property_tree::ptree &config) const final;

		//registers algorithm constructor with the factory.
		//throws runtime exception if registration fails.
		//the return value is always true, used typically to initialize a const bool global variable in a no-name namespace.
		//See italian_search.cpp for registration pattern.
		bool registerAlgorithm(const std::string &name, const maker_t &) noexcept(false);

		//registers all directly usable algorithms in this library with the factory
		//must not be called, it's here only to reference the registering functions so that they're not dropped by the linker
		static void __registerNative();

	  private:

		friend struct Loki::CreateUsingNew<Factory>;
		Factory() = default;

		std::map<std::string, maker_t> _registry;
	};

	using SingleFactory = Ctoolhu::Singleton::Holder<Factory>;

	//creates an algorithm using a single tree with "name" and "config" nodes determining an algorithm.
	//throws if either name or config aren't present in the definition tree.
	IFactory::algorithm_ptr_t create(const boost::property_tree::ptree &definition);

	//returns the NullAlgorithm
	IFactory::algorithm_ptr_t getNullAlgorithm();

} //ns Algorithm

#endif //file guard

