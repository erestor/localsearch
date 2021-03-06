#ifndef _algorithm_tabu_search_included_
#define _algorithm_tabu_search_included_

#include "base.hpp"
#include "config.hpp"
#include "tabu_list.hpp"
#include "../events/tabu_search_events.h"
#include "../events/events.h"
#include "../interface/isolution.h"
#include "../interface/istep.h"
#include <ctoolhu/event/firer.hpp>
#include <ctoolhu/random/selector.hpp>
#include <boost/property_tree/ptree.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace Algorithm::TabuSearch {

	template <class Solution, class Step, class Enable = void>
	class Searcher;

	//Tabu search algorithm
	template <class Solution, class Step>
	class Searcher<Solution, Step, typename std::enable_if<std::is_base_of<IStep<Solution>, Step>::value>::type> : public AlgorithmBase<Solution> {

	  public:

		explicit Searcher(const boost::property_tree::ptree &pt) : _tabuList(pt)
		{
			_config.load(pt);
			_config.maxSteps = pt.get("maxSteps", 500);
			_config.dynamicAdaptationThreshold = pt.get("dynamicAdaptationThreshold", 10);
			_config.interleave = pt.get("interleave", 1);
			_config.fitnessNeutralPreDiscardRatio = pt.get("fitnessNeutralPreDiscardRatio", 0.0f);
			_config.fitnessNeutralPostDiscardRatio = pt.get("fitnessNeutralPostDiscardRatio", 0.9f);
			_config.neighborhood = pt.get<std::string>("neighborhood", "");
		}

		struct Config : Algorithm::Config {
			int maxSteps;
			int dynamicAdaptationThreshold; //how many steps before dynamic adaptation kicks in
			int interleave; //influences the lessons evaluated each step
			float fitnessNeutralPreDiscardRatio; //probability of dicarding a fitness-neutral step before evaluation
			float fitnessNeutralPostDiscardRatio; //probability of dicarding a non-bettering fitness-neutral step after evaluation
			std::string neighborhood; //enables varying neighborhoods in implementation-specific best step getters
		};

		void enableExtensions() noexcept final { _config.extended = true; }
		void disableExtensions() noexcept final { _config.extended = false; }
		const Config &getConfig() const noexcept { return _config; }

		//Assesses given step in the context of the running algorithm to see if it's a candidate for continuation.
		//Returns true if the step can be considered as the next one to take.
		//The current fitness is passed in because the algorithm might be in the middle of tweaking the current solution,
		//which is therefore not safe to be accessed
		bool isAcceptableStep(const Step &step, Fitness currentFitness) const
		{
			return _isAspirationStep(step, currentFitness) || !_tabuList.isTabu(step);
		}

	  protected:

		void executeStep(Solution &solution, const std::shared_ptr<Step> &step)
		{
			step->execute(solution);
			_tabuList.insert(step);
		}

	  private:

		//execute the algorithm
		bool _run(Solution &solution) noexcept(false) final
		{
			_tabuList.clear();
			_bestSolutionPtr = std::make_unique<Solution>(solution);
			Fitness bestFeasible{solution.isFeasible() ? solution.getFitness() : Fitness::worst()}; //holds the fitness of the best feasible solution found so far
			if (!_init(solution))
				return false;

			const int maxSteps{_config.maxSteps * (_config.extended ? 2 : 1)};
			bool improved{false};
			int executedSteps{0};
			int noImprovements{0};
			while (!this->isStopRequested() && !_bestSolutionPtr->getFitness().isZero() && (noImprovements < maxSteps)) {
				noImprovements++;
				auto possibleSteps = _getBestSteps(solution);

				//update the tabu list now so that new entries added when executing the step stay intact for next step
				//also to possibly allow some steps for next move in case no steps have just been found
				_tabuList.shift();

				auto nextStep = _getNextStep(possibleSteps);
				if (nextStep) {
					//can be null if there are no possible steps at this point - might be all tabu
					Ctoolhu::Event::Fire(Events::BeforeStep{&solution});

					//record step data before execution
					std::stringstream s;
					nextStep->dump(s);

					const Fitness expected{solution.getFitness() + nextStep->delta()};
					executeStep(solution, nextStep);
					executedSteps++;
					const Fitness actual{solution.getFitness()};
					if (actual != expected)
						throw std::logic_error("Algorithm::TabuSearch::Searcher::run: unexpected fitness after step execution");

					Ctoolhu::Event::Fire(Events::StepExecuted {
						_config.dynamicAdaptationThreshold,
						&solution,
						s.str(),
						_config.keepFeasible
					});

					Ctoolhu::Event::Fire(Algorithm::Events::CurrentSolutionChanged { &solution, this->elapsedTime() });

					bool foundBest{false};
					if (actual == _bestSolutionPtr->getFitness()) {
						//check for cycling, but only compare structure if the fitness is the same, comparison can be computationally expensive
						if (solution == *_bestSolutionPtr)
							Ctoolhu::Event::Fire(Events::CycleDetected { noImprovements });

						if (noImprovements == maxSteps && _retainsFeasibility(solution)) {
							//this is the last step, accept current solution as the best to improve success chances of chained algorithm,
							//which will continue from this solution as opposed to try with the original solution again
							foundBest = true;
						}
					}
					//check if new best solution was found, in that case store it
					else if (actual < _bestSolutionPtr->getFitness() && _retainsFeasibility(solution)) {
						noImprovements = 0;
						improved = true;
						foundBest = true;
					}
					if (foundBest) {
						*_bestSolutionPtr = solution;
						Ctoolhu::Event::Fire(Algorithm::Events::BestSolutionFound { &solution, this->elapsedTime() });
					}
					if (solution.isFeasible() && actual < bestFeasible) {
						bestFeasible = actual;
						Ctoolhu::Event::Fire(Algorithm::Events::FeasibleSolutionFound { &solution, this->elapsedTime() });
					}
				}
				Ctoolhu::Event::Fire(Events::AfterStep { noImprovements });
			}

			//Cycle is finished with some solution, make sure we use one with the best fitness found, preferring current to the saved best if equal.
			//The reason is that next algorithm in the chain can have a chance to work on a different solution than in the previous cycle.
			if (solution.getFitness() > _bestSolutionPtr->getFitness()) {
				solution = *_bestSolutionPtr;
				Ctoolhu::Event::Fire(Algorithm::Events::CurrentSolutionChanged { &solution, this->elapsedTime() });
			}

			Ctoolhu::Event::Fire(Events::Finished { &solution, executedSteps, this->elapsedTime() });
			return improved;
		}

		//returns random element from the container (next step the solution should take)
		std::shared_ptr<Step> _getNextStep(const std::vector<std::shared_ptr<Step>> &steps) const
		{
			switch (steps.size()) {
				case 0:
					//no valid steps found, can happen for extremely short timetables when all moves are tabu e.g.
					return nullptr;
				case 1:
					return steps[0];
				default: {
					Ctoolhu::Event::Fire(Events::AfterRandomStepChosen { static_cast<int>(steps.size()) });
					return Ctoolhu::Random::Select(steps);
				}
			}
		}

		//aspiration steps are allowed to happen even if they are in the tabu list
		bool _isAspirationStep(const Step &step, Fitness currentFitness) const
		{
			return (currentFitness + step.delta()) < _bestSolutionPtr->getFitness();
		}

		bool _retainsFeasibility(Solution &solution) const
		{
			return !_config.keepFeasible || !_bestSolutionPtr->isFeasible() || solution.isFeasible();
		}

		//Prepare for getting best steps, if necessary.
		//Return false if algorithm cannot run.
		virtual bool _init(Solution &) { return true; }

		//get container with continuation steps for the tabu search
		virtual std::vector<std::shared_ptr<Step>> _getBestSteps(Solution &) const = 0;

		Config _config;
		std::unique_ptr<Solution> _bestSolutionPtr; //holds best solution found so far
		TabuList<Step> _tabuList; //list of tabu steps
	};

} //ns Algorithm::TabuSearch

#endif //file guard
