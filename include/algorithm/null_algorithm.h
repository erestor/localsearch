//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_null_algorithm_included_
#define _algorithm_null_algorithm_included_

#include <algorithm/ialgorithm.h>
#include <chrono>

namespace Algorithm {

	class NullAlgorithm : public IAlgorithm {

	  public:

		const std::string &name() const final { return _name; }
		void setParent(const IAlgorithm *) final {};

		bool start(solution_ptr_t) final { return false; }; //cannot start the null algorithm
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

} //ns Algorithm

#endif //file guard
