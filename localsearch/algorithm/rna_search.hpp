//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_rna_search_included_
#define _algorithm_rna_search_included_

#include "base.hpp"
#include "config.hpp"
#include "../fitness.hpp"
#include "../events/rna_search_events.h"
#include <boost/property_tree/ptree.hpp>

namespace Algorithm {

	namespace RNA {

		//Random Non-Ascendent fitness improvement algorithm
		template <class Solution>
		class Searcher : public AlgorithmBase<Solution> {

		  public:

			explicit Searcher(const boost::property_tree::ptree &pt)
			{
				_config.load(pt);
				_config.maxSteps = pt.get("maxSteps", 1000);
				_config.tickFrequency = max(1, pt.get("tickFrequency", 1000));
			}

			struct Config : Algorithm::Config {
				int maxSteps;
				int tickFrequency; //how many steps between ticks
			};

			void enableExtensions() noexcept final { _config.extended = true; }
			void disableExtensions() noexcept final { _config.extended = false; }
			const Config &getConfig() const noexcept { return _config; }

		  protected:

			bool run(Solution &currentSolution) noexcept(false) override
			{
				int maxSteps = _config.maxSteps;
				if (_config.extended)
					maxSteps *= 2;

				const Fitness starting{currentSolution.getFitness()};
				Fitness bestFeasible{currentSolution.isFeasible() ? starting : Fitness::worst()}; //holds the fitness of the best feasible solution found so far
				bool improved = false;
				int noImprovements = 0;
				int steps = 0;
				while (!isStopRequested() && (int)currentSolution.getFitness() > 0 && (noImprovements < maxSteps)) {
					noImprovements++;
					steps++;
					const Fitness original{currentSolution.getFitness()};
					auto const delta = walk(currentSolution);
					const Fitness actual{currentSolution.getFitness()};
					if (actual != original + delta)
						throw logic_error("Algorithm::RNA::Searcher::run: unexpected fitness delta after walk. Expected " + to_string(delta) + ", got " + to_string(actual - original));

					if (delta > 0)
						throw logic_error("Algorithm::RNA::Searcher::run: search step has positive delta");

					if (delta < 0) {
						noImprovements = 0;
						improved = true;
						Ctoolhu::Event::Fire(Algorithm::Events::CurrentSolutionChanged { &currentSolution, elapsedTime() });
						Ctoolhu::Event::Fire(Algorithm::Events::BestSolutionFound { &currentSolution, elapsedTime() });
					}
					if (currentSolution.isFeasible() && actual < bestFeasible) {
						bestFeasible = actual;
						Ctoolhu::Event::Fire(Algorithm::Events::FeasibleSolutionFound { &currentSolution, elapsedTime() });
					}
					if (steps % _config.tickFrequency == 0)
						Ctoolhu::Event::Fire<Events::Tick>();
				}

				//check postcondition
				assert(currentSolution.getFitness() <= starting && "RNA search should not worsen the solution");

				return improved;
			}

			virtual Fitness::delta_t walk(Solution &) = 0;

		  private:

			Config _config;
		};

	} //ns RNA

} //ns Algorithm

#endif //_rna_included_
