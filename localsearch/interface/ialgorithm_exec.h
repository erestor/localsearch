//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_ialgorithm_exec_included
#define _algorithm_ialgorithm_exec_included

#include "ialgorithm.h"
#include "isolution.h"
#include <type_traits>

namespace Algorithm {

	template <class Solution, class Enable = void>
	class IAlgorithmExec;

	//Executable algorithm
	template <class Solution>
	class IAlgorithmExec<Solution, typename std::enable_if<std::is_base_of<ISolution, Solution>::value>::type> : public IAlgorithm {

	  public:

		IAlgorithmExec(const IAlgorithmExec &) = delete;
		IAlgorithmExec(IAlgorithmExec &&) = delete;
		IAlgorithmExec &operator =(const IAlgorithmExec &) = delete;
		IAlgorithmExec &operator =(IAlgorithmExec &&) = delete;

		//Starts the algorithm (synchronously) and returns when it's finished.
		//Should return true if the solution was changed.
		virtual bool start(Solution &) = 0;

		virtual void setParent(IAlgorithm *) = 0; //nesting support

		virtual void enableExtensions() = 0; //tells algorithm to possibly use extra measures in order to find a feasible solution
		virtual void disableExtensions() = 0;

	  protected:

		IAlgorithmExec() = default;
	};

} //ns Algorithm

#endif
