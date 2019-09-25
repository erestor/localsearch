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

		using solution_ptr_t = std::shared_ptr<ISolution>;

		//returns the name of the algorithm
		virtual const std::string &name() const = 0;

		virtual void setParent(const IAlgorithm *parent) = 0; //nesting support

		//starts the algorithm synchronously
		virtual bool start(solution_ptr_t solutionPtr) = 0;

		virtual void pauseAsync() = 0; //requests pause
		virtual void resumeAsync() = 0; //requests resume

		//requests that the processing should stop at the nearest convenient exit point.
		//I.e. may not stop it immediately!
		virtual void stopAsync() = 0;

		//returns true if stop was requested (probably by calling stopAsync).
		//This is necessary in the interface so that algorithms can be nested (stop request from parent must be accessible)
		virtual bool isStopRequested() const = 0;

		virtual std::chrono::milliseconds elapsedTime() const = 0;

		virtual void enableExtensions() = 0; //tells algorithm to possibly use extra measures in order to find a feasible solution
		virtual void disableExtensions() = 0;

		virtual ~IAlgorithm() = default;
	};

} //ns Algorithm

#endif
