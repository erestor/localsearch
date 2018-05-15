//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_search_included_
#define _algorithm_tabu_search_included_

#include "base.h"
#include "../../src/tabu_list.h"
#include <boost/property_tree/ptree_fwd.hpp>
#include <string>
#include <vector>

namespace Algorithm {

	class ISolution;
	class ISolutionStep;

	namespace TabuSearch {

		//tabu search algorithm
		class Searcher : public AlgorithmBase {

		  public:

			explicit Searcher(const boost::property_tree::ptree &config);
			Searcher(const Searcher &) = delete;

			struct Config : AlgorithmBaseConfig {
				int maxSteps;
				int dynamicAdaptationThreshold;
				std::string neighborhood; //enables varying neighborhoods in implementation-specific best step getters
			};

			void enableExtensions() final;
			void disableExtensions() final;

			const Config &getConfig() const;

			//worst value of step delta (never expected to be reached by an actual step)
			static Fitness::delta_type getWorstDelta();

			//Assesses given step in the context of the running algorithm to see if it's a candidate for continuation.
			//Returns true if the step can be considered as the next one to take.
			//The current fitness is passed in because the algorithm might be in the middle of tweaking the current solution,
			//which is therefore not safe to be accessed
			bool isAcceptableStep(const IStep *, Fitness currentFitness) const;

		  protected:

			//get container with continuation steps for the tabu search
			virtual std::vector<IStep::ptr_t> getBestSteps(ISolution *) const = 0;

		  private:

			//execute the algorithm
			bool run(solution_ptr_t) final;

			//returns random element from the _PossibleSteps container (next step the solution should take)
			IStep::ptr_t _getNextStep(const std::vector<IStep::ptr_t> &) const;

			//aspiration steps are allowed to happen even if they are in the tabu list
			bool _isAspirationStep(const ISolutionStep *, Fitness currentFitness) const;

			bool _retainsFeasibility(ISolution *) const;

			Config _config;
			std::unique_ptr<ISolution> _bestSolutionPtr; //holds best solution found so far
			std::unique_ptr<ISolution> _feasibleSolutionPtr; //holds best feasible solution found so far
			TabuList _tabuList; //list of tabu steps
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
