//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_tabu_search_included_
#define _algorithm_tabu_search_included_

#include "base.h"
#include "itabusearch.h"
#include "../../src/tabu_list.h"
#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>
#include <vector>

namespace Algorithm {

	class ISolution;
	class ISolutionStep;

	namespace TabuSearch {

		//tabu search algorithm
		class Searcher : public AlgorithmBase, public ISearcher {

		  public:

			typedef std::unique_ptr<IStep> step_ptr_type;

			const Fitness::delta_type &GetWorstDelta() const final;

		  protected:

			Searcher(const boost::property_tree::ptree &config);

			//get container with continuation steps for the tabu search
			virtual std::vector<step_ptr_type> GetBestSteps() const = 0;

			//assesses given step in the context of the running algorithm to see if it's a candidate for continuation
			bool IsAcceptableStep(const IStep *, const Fitness &currentFitness) const override;

			ISolution *_currentSolutionPtr;

		  private:

			struct Config {
				int maxSteps;
				int dynamicAdaptationThreshold;
			};

			//execute the algorithm
			bool Run(solution_ptr_type) final;

			//returns random element from the _PossibleSteps container (next step the solution should take)
			step_ptr_type _GetNextStep(std::vector<step_ptr_type> &) const;

			//aspiration steps are allowed to happen even if they are in the tabu list
			bool _IsAspirationStep(const ISolutionStep *, const Fitness &currentFitness) const;

			std::unique_ptr<ISolution> _bestSolutionPtr; //holds best solution found so far
			TabuList _tabuList; //list of tabu steps
			Config _config;
		};

	} //ns TabuSearch

} //ns Algorithm

#endif //file guard
