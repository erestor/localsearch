//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include "null_algorithm.hpp"
#include <algorithm/factory.h>
#include <algorithm/italian_search.h>
#include <stdexcept>
#include <string>

using namespace boost::property_tree;
using namespace std;

namespace Algorithm {

IFactory::algorithm_ptr_t Factory::CreateAlgorithm(const string &name, const ptree &config) const
{
	auto it = _registry.find(name);
	if (it == _registry.end())
		return GetNullAlgorithm();

	return it->second(config);
}

bool Factory::Register(const string &name, const maker_t &maker)
{
	bool success = _registry.emplace(name, maker).second;
	if (!success)
		throw runtime_error("Algorithm::Factory::Register: failed to register '" + name + "' with algorithm factory");

	return true;
}

void Factory::__RegisterNative()
{
	ItalianSearch::Private::Register();
	throw logic_error(__FUNCTION__ + string(" must never be called"));
}

IFactory::algorithm_ptr_t Create(const ptree &definition)
{
	return SingleFactory::Instance().CreateAlgorithm(
		definition.get<string>("name"),
		definition.get_child("config")
	);
}

IFactory::algorithm_ptr_t GetNullAlgorithm()
{
	return make_unique<NullAlgorithm>();
}

} //ns Algorithm
