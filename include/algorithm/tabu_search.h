//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_search_included_
#define _algorithm_tabu_search_included_

#include "base.h"
#include "../../src/tabu_list.h"
#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Algorithm {

	class ISolution;
	class ISolutionStep;

	namespace TabuSearch {

		//tabu search algorithm
		class Searcher : public AlgorithmBase {

		  public:

			typedef std::unique_ptr<IStep> step_ptr_type;

			struct Config : AlgorithmBaseConfig {
				int maxSteps;
				int dynamicAdaptationThreshold;
				std::string neighborhood; //enables varying neighborhoods in implementation-specific best step getters
			};

			void EnableExtensions() final;
			void DisableExtensions() final;

			const Config &GetConfig() const;

			//worst value of step delta (never expected to be reached by an actual step)
			const Fitness::delta_type &GetWorstDelta() const;

			//Assesses given step in the context of the running algorithm to see if it's a candidate for continuation.
			//Returns true if the step can be considered as the next one to take.
			//The current fitness is passed in because the algorithm might be in the middle of tweaking the current solution,
			//which is therefore not safe to be accessed
			bool IsAcceptableStep(const IStep *, const Fitness &currentFitness) const;

		  protected:

			Searcher(const boost::property_tree::ptree &config);

			//get container with continuation steps for the tabu search
			virtual std::vector<step_ptr_type> GetBestSteps() const = 0;

			ISolution *_currentSolutionPtr;
			Config _config;

		  private:

			//execute the algorithm
			bool Run(solution_ptr_type) final;

			//returns random element from the _PossibleSteps container (next step the solution should take)
			step_ptr_type _GetNextStep(std::vector<step_ptr_type> &) const;

			//aspiration steps are allowed to happen even if they are in the tabu list
			bool _IsAspirationStep(const ISolutionStep *, const Fitness &currentFitness) const;

			bool _CurrentSolutionRetainsFeasibility() const;

			std::unique_ptr<ISolution> _bestSolutionPtr; //holds best solution found so far
			std::unique_ptr<ISolution> _feasibleSolutionPtr; //holds best feasible solution found so far
			TabuList _tabuList; //list of tabu steps
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
