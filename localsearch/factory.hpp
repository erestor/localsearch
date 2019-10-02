//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_factory_included_
#define _algorithm_factory_included_

#include "interface/ialgorithm_exec.h"
#include "algorithm/null_algorithm.hpp"
#include <ctoolhu/singleton/holder.hpp>
#include <loki/Singleton.h>
#include <boost/property_tree/ptree.hpp>
#include <map>
#include <memory>
#include <stdexcept>

namespace Algorithm {

	template <class Solution> std::unique_ptr<IAlgorithmExec<Solution>> getNullAlgorithm();

	namespace Private {

		//producer of arbitrary algorithm objects
		template <class Solution>
		class Factory {

		  public:

			using algorithm_ptr_t = std::unique_ptr<IAlgorithmExec<Solution>>;
			using maker_t = std::function<algorithm_ptr_t(const boost::property_tree::ptree &)>;

			//returns new algorithm registered under the name, or NullAlgorithm instance if not found.
			algorithm_ptr_t createAlgorithm(const std::string &name, const boost::property_tree::ptree &config, IAlgorithm *parent) const
			{
				auto it = _registry.find(name);
				if (it == _registry.end())
					return getNullAlgorithm<Solution>();

				auto alg = it->second(config);
				if (parent)
					alg->setParent(parent);

				return alg;
			}

			//Registers algorithm constructor with the factory.
			//Throws runtime exception if registration fails.
			//The return value is always true, used typically to initialize a const bool global variable in a no-name namespace (which is the registration).
			bool registerAlgorithm(const std::string &name, const maker_t &maker) noexcept(false)
			{
				bool success = _registry.emplace(name, maker).second;
				if (!success)
					throw std::runtime_error("Algorithm::Factory::registerAlgorithm: failed to register '" + name + "' with algorithm factory");

				return true;
			}

		  private:

			friend struct Loki::CreateUsingNew<Factory>;
			Factory() = default;

			std::map<std::string, maker_t> _registry;
		};

	} //ns Private

	template <class Solution>
	using SingleFactory = Ctoolhu::Singleton::Holder<Private::Factory<Solution>>;

	//creates an algorithm using a single tree with "name" and "config" nodes determining an algorithm.
	//throws if either name or config aren't present in the definition tree.
	template <class Solution>
	auto create(const boost::property_tree::ptree &definition, IAlgorithm *parent)
	{
		return SingleFactory<Solution>::Instance().createAlgorithm(
			definition.get<std::string>("name"),
			definition.get_child("config"),
			parent
		);
	}

	//returns the NullAlgorithm
	template <class Solution>
	std::unique_ptr<IAlgorithmExec<Solution>> getNullAlgorithm()
	{
		return std::make_unique<NullAlgorithm<Solution>>();
	}

} //ns Algorithm

#endif //file guard
