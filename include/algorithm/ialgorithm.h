//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_ialgorithm_included
#define _algorithm_ialgorithm_included

#include <chrono>
#include <memory>
#include <string>

namespace Algorithm {

	class ISolution;

	class IAlgorithm {

	  public:

		typedef std::shared_ptr<ISolution> solution_ptr_t;

		virtual void setParent(const IAlgorithm *parent) = 0; //nested algorithms support

		virtual bool start(solution_ptr_t solutionPtr) = 0;
		virtual void stop() = 0; //signals the algorithm that the processing should stop at the nearest exit point
		virtual void pause() = 0;
		virtual void resume() = 0;
		virtual bool isStopRequested() const = 0;

		virtual void enableExtensions() = 0;	//tells algorithm to use extra measures in order to find a feasible solution
		virtual void disableExtensions() = 0;
	
		virtual std::chrono::milliseconds elapsedTime() const = 0;

		virtual const std::string &name() const = 0; //should return the name of the algorithm

		virtual ~IAlgorithm() {};
	};

} //ns Algorithm

#endif
