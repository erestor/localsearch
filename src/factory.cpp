//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/factory.h>
#include <algorithm/italian_search.h>
#include <algorithm/null_algorithm.h>
#include <stdexcept>
#include <string>

using namespace boost::property_tree;
using namespace std;

namespace Algorithm {

IFactory::algorithm_ptr_t Factory::createAlgorithm(const string &name, const ptree &config) const
{
	auto it = _registry.find(name);
	if (it == _registry.end())
		return getNullAlgorithm();

	return it->second(config);
}

bool Factory::registerAlgorithm(const string &name, const maker_t &maker)
{
	bool success = _registry.emplace(name, maker).second;
	if (!success)
		throw runtime_error("Algorithm::Factory::registerAlgorithm: failed to register '" + name + "' with algorithm factory");

	return true;
}

void Factory::__registerNative()
{
	ItalianSearch::Private::_register();
	throw logic_error(__FUNCTION__ + string(" must never be called"));
}

IFactory::algorithm_ptr_t create(const ptree &definition)
{
	return SingleFactory::Instance().createAlgorithm(
		definition.get<string>("name"),
		definition.get_child("config")
	);
}

IFactory::algorithm_ptr_t getNullAlgorithm()
{
	return make_unique<NullAlgorithm>();
}

} //ns Algorithm
