//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/italian_search.h>
#include <algorithm/factory.h>
#include <algorithm/isolution.h>
#include <boost/property_tree/ptree.hpp>

using namespace boost::property_tree;
using namespace std;

namespace Algorithm { namespace ItalianSearch {

namespace Private {

bool Register()
{
	return SingleFactory::Instance().Register("italian_search",
		[](const ptree &pt) {
			return make_unique<ItalianSearch>(pt);
		});
}

} //ns Private

namespace {
	//register with algorithm factory at program start-up
	const bool registered = Private::Register();
}

ItalianSearch::ItalianSearch(const ptree &configPt)
{
	_config.cycles = configPt.get("cycles", 4);

	auto const &initNode = configPt.get_child("initial");
	_config.initial = make_pair(initNode.get("name", "generation"), initNode.get_child("config"));

	vector<string> defaults {
		"room_rna",
		"room_tabu",
		"time_rna",
		"time_tabu"
	};
	unsigned int i = 0;
	for (auto const &node : configPt.get_child("algorithms")) {
		_config.algorithms.emplace_back(
			node.second.get("name", i < defaults.size() ? defaults[i] : ""),
			node.second.get_child("config")
		);
		++i;
	}
}

const string &ItalianSearch::Name() const
{
	static string name { "Italian search" };
	return name;
}

bool ItalianSearch::Run(solution_ptr_type solutionPtr)
{
	auto initialFitness = solutionPtr->GetFitness();
	{
		//prelude is the generation algorithm
		auto initialAlg = SingleFactory::Instance().CreateAlgorithm(_config.initial.first, _config.initial.second);
		initialAlg->SetParent(this);
		initialAlg->Start(solutionPtr);
	}

	vector<unique_ptr<IAlgorithm>> algorithms;
	for (auto const &algDef : _config.algorithms) {
		auto alg = SingleFactory::Instance().CreateAlgorithm(algDef.first, algDef.second);
		alg->SetParent(this);
		algorithms.push_back(move(alg));
	}

	int idleCycles = 0;
	while (!IsStopRequested() && (int)solutionPtr->GetFitness() > 0 && (_config.cycles < 0 || idleCycles < _config.cycles)) {
		idleCycles++;
		for (auto const &alg : algorithms) {
			if (alg->Start(solutionPtr))
				idleCycles = 0;
		}
	}	
	return solutionPtr->GetFitness() < initialFitness;
}

} } //ns Algorithm::ItalianSearch
