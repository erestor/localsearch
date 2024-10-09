#ifndef _algorithm_chained_search_included_
#define _algorithm_chained_search_included_

#include "base.hpp"
#include "config.hpp"
#include "../factory.hpp"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <utility>
#include <vector>

namespace Algorithm::ChainedSearch {

	//chained local search
	//Use config to inject algorithms and their configurations as necessary.
	template <class Solution>
	class Searcher : public AlgorithmBase<Solution> {

		using algorithm_ptr_t = typename Private::Factory<Solution>::algorithm_ptr_t;

	  public:

		explicit Searcher(const boost::property_tree::ptree &pt)
		{
			_config.load(pt);
			_config.repeat = pt.get("repeat", 1);
			_config.cycles = pt.get("cycles", 4);

			if (pt.count("initial")) {
				auto const &initNode = pt.get_child("initial");
				_config.initial = std::pair{initNode.get("name", "generation"), initNode.get_child("config")};
			}

			for (auto const &node : pt.get_child("algorithms")) {
				_config.algorithms.emplace_back(
					node.second.get<std::string>("name"),
					node.second.get_child("config")
				);
			}

			for (auto &algDef : _config.algorithms) {
				_config.propagate(algDef.second);
				auto alg = SingleFactory<Solution>::Instance().createAlgorithm(algDef.first, algDef.second, this);
				_algorithms.push_back(std::move(alg));
			}
		}

		void enableExtensions() final
		{
			for (auto const &alg : _algorithms)
				alg->enableExtensions();

			Ctoolhu::Event::Fire<Algorithm::Events::ExtensionsEnabled>();
		}

		void disableExtensions() final
		{
			for (auto const &alg : _algorithms)
				alg->disableExtensions();

			Ctoolhu::Event::Fire<Algorithm::Events::ExtensionsDisabled>();
		}

	  private:

		using algorithm_def_t = std::pair<std::string, boost::property_tree::ptree>;

		struct Config : Algorithm::Config {
			int repeat; //indicates how many times should the algorithm be repeated from the starting point (if solution isn't found)
			int cycles; //indicates how many times the child algorithms will be run in a loop on the same solution in case of no improvement
			algorithm_def_t initial; //name of initial algorithm run once (usually some kind of a generation algorithm)
			std::vector<algorithm_def_t> algorithms;
				//chain of algorithms run in cycles until stopping condition is met
		};

		bool _run(Solution &solution) final
		{
			const Fitness starting{solution.getFitness()};
			auto storedSolutionPtr = std::make_unique<Solution>(solution); //don't use the stack as we don't know the size of the solution

			algorithm_ptr_t initialAlgorithm;
			if (!_config.initial.first.empty())
				initialAlgorithm = SingleFactory<Solution>::Instance().createAlgorithm(_config.initial.first, _config.initial.second, this);

			while (!this->isStopRequested() && !solution.getFitness().isZero() && _config.repeat-- > 0) {
				solution = *storedSolutionPtr;
				if (initialAlgorithm)
					initialAlgorithm->start(solution);

				bool extended{_config.extended};
				if (extended)
					enableExtensions();

				int idleCycles{0};
				while (!this->isStopRequested() && !solution.getFitness().isZero()) {
					idleCycles++;
					for (auto const &alg : _algorithms) {
						if (alg->start(solution))
							idleCycles = 0;
					}

					//apply some meta-logic
					if (!_config.extended && solution.isFeasible()) {
						//save time by disabling extensions once a feasible solution is found
						extended = false;
						disableExtensions();
					}
					if (!extended && !solution.isFeasible() && idleCycles == _config.cycles) {
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
			return solution.getFitness() < starting;
		}

		std::vector<algorithm_ptr_t> _algorithms;
		Config _config;
	};

} //ns Algorithm::ChainedSearch

#endif //file guard
