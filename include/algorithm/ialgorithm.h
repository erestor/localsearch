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

		virtual void SetParent(const IAlgorithm *parent) = 0; //nested algorithms support

		virtual bool Start(const solution_ptr_t &solutionPtr) = 0;
		virtual void Stop() = 0; //signals the algorithm that the processing should stop at the nearest exit point
		virtual void Pause() = 0;
		virtual void Resume() = 0;
		virtual bool IsStopRequested() const = 0;

		virtual void EnableExtensions() = 0;	//tells algorithm to use extra measures in order to find a feasible solution
		virtual void DisableExtensions() = 0;
	
		virtual std::chrono::milliseconds ElapsedTime() const = 0;

		virtual const std::string &Name() const = 0; //should return the name of the algorithm

		virtual ~IAlgorithm() {};
	};

} //ns Algorithm

#endif
