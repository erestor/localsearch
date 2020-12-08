#ifndef _algorithm_rna_search_included_
#define _algorithm_rna_search_included_

#include "base.hpp"
#include "config.hpp"
#include "../fitness.hpp"
#include "../events/rna_search_events.h"
#include <boost/property_tree/ptree.hpp>
#include <algorithm>
#include <stdexcept>

namespace Algorithm::RNA {

	//Random Non-Ascendent search algorithm
	template <class Solution>
	class Searcher : public AlgorithmBase<Solution> {

	  public:

		explicit Searcher(const boost::property_tree::ptree &pt)
		{
			_config.load(pt);
			_config.maxSteps = pt.get("maxSteps", 1000);
			_config.tickFrequency = std::max(1, pt.get("tickFrequency", 1000));
		}

		struct Config : Algorithm::Config {
			int maxSteps;
			int tickFrequency; //how many steps between ticks
		};

		void enableExtensions() noexcept final { _config.extended = true; }
		void disableExtensions() noexcept final { _config.extended = false; }
		const Config &getConfig() const noexcept { return _config; }

	  private:

		bool _run(Solution &solution) noexcept(false) final
		{
			const Fitness startingFitness{solution.getFitness()};
			Fitness bestFeasible{solution.isFeasible() ? startingFitness : Fitness::worst()}; //holds the fitness of the best feasible solution found so far
			if (!_init(solution))
				return false;

			const int maxSteps{_config.maxSteps * (_config.extended ? 2 : 1)};
			bool improved{false};
			int executedSteps{0};
			int noImprovements{0};
			while (!this->isStopRequested() && !solution.getFitness().isZero() && (noImprovements < maxSteps)) {
				noImprovements++;
				executedSteps++;
				const Fitness original{solution.getFitness()};
				auto const delta = _walk(solution);
				const Fitness actual{solution.getFitness()};
				if (actual != original + delta)
					throw std::logic_error("Algorithm::RNA::Searcher::run: unexpected fitness delta after walk. Expected " + std::to_string(delta) + ", got " + std::to_string(actual - original));

				if (delta > 0)
					throw std::logic_error("Algorithm::RNA::Searcher::run: search step has positive delta");

				if (delta < 0) {
					noImprovements = 0;
					improved = true;
					Ctoolhu::Event::Fire(Algorithm::Events::CurrentSolutionChanged { &solution, this->elapsedTime() });
					Ctoolhu::Event::Fire(Algorithm::Events::BestSolutionFound { &solution, this->elapsedTime() });
				}
				if (solution.isFeasible() && actual < bestFeasible) {
					bestFeasible = actual;
					Ctoolhu::Event::Fire(Algorithm::Events::FeasibleSolutionFound { &solution, this->elapsedTime() });
				}
				if (executedSteps % _config.tickFrequency == 0)
					Ctoolhu::Event::Fire<Events::Tick>();
			}
			assert(solution.getFitness() <= startingFitness && "RNA search should not worsen the solution");
			return improved;
		}

		//Prepare for walking, if necessary.
		//Return false if algorithm cannot run.
		virtual bool _init(Solution &) { return true; }

		virtual Fitness::delta_t _walk(Solution &) const = 0;

		Config _config;
	};

} //ns Algorithm::RNA

#endif //file guard
