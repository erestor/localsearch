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

	//Random Non-Ascendent fitness improvement algorithm
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

		bool _run(Solution &currentSolution) noexcept(false) final
		{
			if (!_init(currentSolution))
				return false;

			int maxSteps = _config.maxSteps;
			if (_config.extended)
				maxSteps *= 2;

			const Fitness starting{currentSolution.getFitness()};
			Fitness bestFeasible{currentSolution.isFeasible() ? starting : Fitness::worst()}; //holds the fitness of the best feasible solution found so far
			bool improved = false;
			int noImprovements = 0;
			int steps = 0;
			while (!this->isStopRequested() && !currentSolution.getFitness().isZero() && (noImprovements < maxSteps)) {
				noImprovements++;
				steps++;
				const Fitness original{currentSolution.getFitness()};
				auto const delta = _walk(currentSolution);
				const Fitness actual{currentSolution.getFitness()};
				if (actual != original + delta)
					throw std::logic_error("Algorithm::RNA::Searcher::run: unexpected fitness delta after walk. Expected " + std::to_string(delta) + ", got " + std::to_string(actual - original));

				if (delta > 0)
					throw std::logic_error("Algorithm::RNA::Searcher::run: search step has positive delta");

				if (delta < 0) {
					noImprovements = 0;
					improved = true;
					Ctoolhu::Event::Fire(Algorithm::Events::CurrentSolutionChanged { &currentSolution, this->elapsedTime() });
					Ctoolhu::Event::Fire(Algorithm::Events::BestSolutionFound { &currentSolution, this->elapsedTime() });
				}
				if (currentSolution.isFeasible() && actual < bestFeasible) {
					bestFeasible = actual;
					Ctoolhu::Event::Fire(Algorithm::Events::FeasibleSolutionFound { &currentSolution, this->elapsedTime() });
				}
				if (steps % _config.tickFrequency == 0)
					Ctoolhu::Event::Fire<Events::Tick>();
			}
			assert(currentSolution.getFitness() <= starting && "RNA search should not worsen the solution");
			return improved;
		}

		//Prepare for walking, if necessary.
		//Return false if algorithm cannot run.
		virtual bool _init(Solution &) { return true; };

		virtual Fitness::delta_t _walk(Solution &) = 0;

		Config _config;
	};

} //ns Algorithm::RNA

#endif //_rna_included_
