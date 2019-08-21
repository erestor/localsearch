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

bool _register()
{
	return SingleFactory::Instance().registerAlgorithm("italian_search",
		[](const ptree &pt) {
			return make_unique<ItalianSearch>(pt);
		});
}

} //ns Private

namespace {
	//register with algorithm factory at program start-up
	const bool registered = Private::_register();
}

ItalianSearch::ItalianSearch(const ptree &configPt)
{
	_config.load(configPt);
	_config.repeat = configPt.get("repeat", 1);
	_config.cycles = configPt.get("cycles", 4);

	if (configPt.count("initial")) {
		auto const &initNode = configPt.get_child("initial");
		_config.initial = make_pair(initNode.get("name", "generation"), initNode.get_child("config"));
	}

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
		_config.propagate(algDef.second);
		auto alg = SingleFactory::Instance().createAlgorithm(algDef.first, algDef.second);
		alg->setParent(this);
		_algorithms.push_back(move(alg));
	}
}

const string &ItalianSearch::name() const
{
	static string name { "Italian search" };
	return name;
}

void ItalianSearch::enableExtensions()
{
	for (auto const &alg : _algorithms)
		alg->enableExtensions();

	Ctoolhu::Event::Fire<Algorithm::Events::ExtensionsEnabled>();
}

void ItalianSearch::disableExtensions()
{
	for (auto const &alg : _algorithms)
		alg->disableExtensions();
}

bool ItalianSearch::run(solution_ptr_t solutionPtr)
{
	const Fitness starting{solutionPtr->getFitness()};
	unique_ptr<ISolution> storedSolutionPtr(solutionPtr->clone());

	IFactory::algorithm_ptr_t initialAlgorithm{nullptr};
	if (!_config.initial.first.empty()) {
		initialAlgorithm = SingleFactory::Instance().createAlgorithm(_config.initial.first, _config.initial.second);
		initialAlgorithm->setParent(this);
	}

	while (!isStopRequested() && (int)solutionPtr->getFitness() > 0 && _config.repeat-- > 0) {
		storedSolutionPtr->copyTo(solutionPtr.get());
		if (initialAlgorithm)
			initialAlgorithm->start(solutionPtr);

		bool extended{_config.extended};
		if (extended)
			enableExtensions();

		int idleCycles{0};
		while (!isStopRequested() && (int)solutionPtr->getFitness() > 0) {
			idleCycles++;
			for (auto const &alg : _algorithms) {
				if (alg->start(solutionPtr))
					idleCycles = 0;
			}

			//apply some meta-logic
			if (!_config.extended && solutionPtr->isFeasible()) {
				//save time by disabling extensions once a feasible solution is found
				extended = false;
				disableExtensions();
			}
			if (!extended && !solutionPtr->isFeasible() && idleCycles == _config.cycles) {
				//we're at the end and feasible solution wasn't found - re-run algorithms with extensions enabled
				extended = true;
				enableExtensions();
				idleCycles = 0;

				//reduce the number of cycles to save time - if a better solution isn't found with extensions then it's probably worthless to continue cycling a lot
				if (_config.cycles > 1)
					_config.cycles /= 2;
			}

			if (_config.cycles >= 0 && _config.cycles <= idleCycles)
				break;
		}
	}
	return solutionPtr->getFitness() < starting;
}

} } //ns Algorithm::ItalianSearch
