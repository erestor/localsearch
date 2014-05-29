//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _tabu_search_swap_included_
#define _tabu_search_swap_included_

#include "tabu_search_step.h"
#include <utility>

namespace Algorithm {

	namespace TabuSearch {

		//swapping two lessons actually means moving both of them at the same time,
		//so the swap is defined as a pair of moves
		template <class Move>
		class Swap : public Step {

			typedef std::pair<Move, Move> moves_pair_type;

		  public:

			bool IsAtomicForTabu() const final
			{
				return false;
			}

			bool IsUndo(const IStep *stepPtr) const final
			{
				return _movesPair.first.IsUndo(stepPtr) || _movesPair.second.IsUndo(stepPtr);
			}

			bool IsEqualForShortTermTabu(const IStep *stepPtr) const final
			{
				return _movesPair.first.IsEqualForShortTermTabu(stepPtr) || _movesPair.second.IsEqualForShortTermTabu(stepPtr);
			}

			const Move &GetFirstMove() const
			{
				return _movesPair.first;
			}

			const Move &GetSecondMove() const
			{
				return _movesPair.second;
			}

		  protected:

			Swap(const moves_pair_type &movesPair) : _movesPair(movesPair) {}

			const moves_pair_type _movesPair;
		};

	}
}

#endif //file guard
