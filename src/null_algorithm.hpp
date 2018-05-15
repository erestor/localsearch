//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/ialgorithm.h>
#include <chrono>

namespace Algorithm {

	class NullAlgorithm : public IAlgorithm {

	  public:

		void setParent(const IAlgorithm *) final {};
		bool start(const solution_ptr_t &) final { return false; }; //cannot start the null algorithm
		void stop() final {};
		void pause() final {};
		void resume() final {};
		bool isStopRequested() const final { return true; }
		void enableExtensions() final {};
		void disableExtensions() final {};
		std::chrono::milliseconds elapsedTime() const final { return std::chrono::milliseconds(0); }
		const std::string &name() const final { static std::string name { "Null algorithm" }; return name; }
	};

} //ns Algorithm
