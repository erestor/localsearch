//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_fitness_included_
#define _algorithm_fitness_included_

#include <ctoolhu/typesafe/id.hpp>
#include <cassert>
#include <limits>
#include <stdexcept>

namespace Algorithm {

	namespace Private {

		template <class RequestingObject, typename StoredType>
		class FitnessImpl : public Ctoolhu::TypeSafe::Id<RequestingObject, StoredType, Ctoolhu::TypeSafe::ExplicitConversion> {

			typedef FitnessImpl<RequestingObject, StoredType> self_t;
			typedef Ctoolhu::TypeSafe::Id<RequestingObject, StoredType, Ctoolhu::TypeSafe::ExplicitConversion> base_t;

		  public:

			typedef typename std::make_signed<StoredType>::type delta_type;

			FitnessImpl() : base_t(0) {}

			explicit FitnessImpl(StoredType id) : base_t(id)
			{
				_verify();
			}

			static self_t worst()
			{
				return self_t{std::numeric_limits<StoredType>::max()};
			}

			//worst value of delta (never expected to be reached by an actual step)
			static delta_type worstDelta()
			{
				return std::numeric_limits<delta_type>::max();
			}

			self_t operator +(self_t fitness) const
			{
				return self_t{this->_id + fitness._id};
			}

			self_t operator +(delta_type delta) const
			{
				return self_t{this->_id + delta};
			}

			delta_type operator -(self_t fitness) const
			{
				return this->_id - fitness._id;
			}

			void operator +=(self_t fitness)
			{
				this->_id += fitness._id;
			}

			void operator +=(int penalty)
			{
				this->_id += penalty;
				_verify();
			}

			void operator -=(self_t fitness)
			{
				this->_id -= fitness._id;
				_verify();
			}

			void operator -=(int penalty)
			{
				this->_id -= penalty;
				_verify();
			}

		  private:

			void _verify()
			{
				assert(this->_id >= 0 && "Algorithm::Private::FitnessImpl::_Verify: Fitness can't be negative");
			}
		};

		class FitnessDummy;
		class AdaptedFitnessDummy;

	} //ns Private

	typedef Private::FitnessImpl<Private::FitnessDummy, int> Fitness;
	typedef Private::FitnessImpl<Private::AdaptedFitnessDummy, int> AdaptedFitness;

} //ns Algorithm

#endif //file guard
