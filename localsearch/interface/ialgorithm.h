//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_ialgorithm_included
#define _algorithm_ialgorithm_included

#include <chrono>
#include <string>

namespace Algorithm {

	//This is a non-executable algorithm, but still useful, because it doesn't have to know the type of the solution
	//but still provides access to useful operations.
	//The rationale behind having this is to provide an interface to an algorithm that is already running.
	class IAlgorithm {

	  public:

		virtual ~IAlgorithm() = default;

		//returns the name of the algorithm
		virtual const std::string &name() const = 0;

		virtual void pauseAsync() = 0; //requests pause
		virtual void resumeAsync() = 0; //requests resume

		//requests that the processing should stop at the nearest convenient exit point (may not stop it immediately)
		virtual void stopAsync() = 0;

		//Returns true if stop was requested (probably by calling stopAsync).
		//This is necessary in the interface so that algorithms can be nested (stop request from parent must be accessible)
		virtual bool isStopRequested() const = 0;

		virtual std::chrono::milliseconds elapsedTime() const = 0;
	};

} //ns Algorithm

#endif
