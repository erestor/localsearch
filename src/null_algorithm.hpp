//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#include <algorithm/ialgorithm.h>
#include <chrono>

namespace Algorithm {

	class NullAlgorithm : public IAlgorithm {

	  public:

		void SetParent(const IAlgorithm *) final {};
		bool Start(const solution_ptr_type &) final { return false; }; //cannot start the null algorithm
		void Stop() final {};
		void Pause() final {};
		void Resume() final {};
		bool IsStopRequested() const final { return true; }	
		std::chrono::milliseconds ElapsedTime() const final { return std::chrono::milliseconds(0); }
		const std::string &Name() const final { static std::string name { "Null algorithm" }; return name; }
	};

} //ns Algorithm
