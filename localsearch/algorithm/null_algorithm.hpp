//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_null_algorithm_included_
#define _algorithm_null_algorithm_included_

#include "../interface/ialgorithm_exec.h"
#include <chrono>
#include <string>

namespace Algorithm {

	template <class Solution>
	class NullAlgorithm : public IAlgorithmExec<Solution> {

	  public:

		const std::string &name() const final { return _name; }
		void setParent(IAlgorithm *) final {};

		bool start(Solution &) final { return false; }; //cannot start the null algorithm
		void pauseAsync() final {};
		void resumeAsync() final {};
		void stopAsync() final {};
		bool isStopRequested() const final { return true; }

		std::chrono::milliseconds elapsedTime() const final { return std::chrono::milliseconds(0); }

		void enableExtensions() final {};
		void disableExtensions() final {};

	  private:

		static const std::string _name;
	};

	template <class Solution>
	const std::string NullAlgorithm<Solution>::_name{"Null algorithm"};

} //ns Algorithm

#endif //file guard
