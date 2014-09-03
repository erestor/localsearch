//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include "null_algorithm.hpp"
#include <algorithm/factory.h>
#include <algorithm/italian_search.h>
#include <stdexcept>

using namespace boost::property_tree;
using namespace std;

namespace Algorithm {

IFactory::ialgorithm_ptr_type Factory::CreateAlgorithm(const string &name, const ptree &config) const
{
	auto it = _registry.find(name);
	if (it == _registry.end())
		return GetNullAlgorithm();

	return it->second(config);
}

bool Factory::Register(const string &name, const maker_type &maker)
{
	bool success = _registry.emplace(name, maker).second;
	if (!success)
		throw runtime_error("failed to register '" + name + "' with algorithm factory");

	return true;
}

void Factory::__RegisterNative()
{
	ItalianSearch::Private::Register();
	throw logic_error(__FUNCTION__ " must never be called");
}

IFactory::ialgorithm_ptr_type Create(const ptree &definition)
{
	return SingleFactory::Instance().CreateAlgorithm(
		definition.get<string>("name"),
		definition.get_child("config")
	);
}

IFactory::ialgorithm_ptr_type GetNullAlgorithm()
{
	return make_unique<NullAlgorithm>();
}

} //ns Algorithm
