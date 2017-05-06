//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/italian_search.h>
#include <algorithm/events.h>
#include <algorithm/factory.h>
#include <algorithm/isolution.h>
#include <ctoolhu/event/firer.hpp>
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
	_config.Load(configPt);
	_config.repeat = configPt.get("repeat", 1);
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

	for (auto &algDef : _config.algorithms) {
		_config.Propagate(algDef.second);
		auto alg = SingleFactory::Instance().CreateAlgorithm(algDef.first, algDef.second);
		alg->SetParent(this);
		_algorithms.push_back(move(alg));
	}
}

const string &ItalianSearch::Name() const
{
	static string name { "Italian search" };
	return name;
}

void ItalianSearch::EnableExtensions()
{
	for (auto const &alg : _algorithms)
		alg->EnableExtensions();

	Ctoolhu::Event::Fire<Algorithm::Events::ExtensionsEnabled>();
}

void ItalianSearch::DisableExtensions()
{
	for (auto const &alg : _algorithms)
		alg->DisableExtensions();
}

bool ItalianSearch::Run(solution_ptr_type solutionPtr)
{
	auto initialFitness = solutionPtr->GetFitness();
	auto storedSolution = solutionPtr->Clone();
	bool extended = _config.extended;
	auto initialAlgorithm = SingleFactory::Instance().CreateAlgorithm(_config.initial.first, _config.initial.second);
	initialAlgorithm->SetParent(this);

	while (!IsStopRequested() && (int)solutionPtr->GetFitness() > 0 && _config.repeat-- > 0) {
		storedSolution->CopyTo(solutionPtr.get());
		initialAlgorithm->Start(solutionPtr);
		if (extended)
			EnableExtensions();

		int idleCycles = 0;
		while (!IsStopRequested() && (int)solutionPtr->GetFitness() > 0 && (_config.cycles < 0 || idleCycles < _config.cycles)) {
			idleCycles++;
			for (auto const &alg : _algorithms) {
				if (alg->Start(solutionPtr))
					idleCycles = 0;
			}

			//apply some meta-logic
			if (!_config.extended && solutionPtr->IsFeasible()) {
				//save time by disabling extensions once a feasible solution is found
				extended = false;
				DisableExtensions();
			}
			if (!extended && !solutionPtr->IsFeasible() && idleCycles == _config.cycles) {
				//we're at the end and feasible solution wasn't found - re-run algorithms with extensions enabled
				extended = true;
				EnableExtensions();
				idleCycles = 0;

				//reduce the number of cycles to save time - if a better solution isn't found with extensions then it's probably worthless to continue cycling a lot
				if (_config.cycles > 1)
					_config.cycles /= 2;
			}
		}
	}
	return solutionPtr->GetFitness() < initialFitness;
}

} } //ns Algorithm::ItalianSearch
