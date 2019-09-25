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

			using self_t = FitnessImpl<RequestingObject, StoredType>;
			using base_t = Ctoolhu::TypeSafe::Id<RequestingObject, StoredType, Ctoolhu::TypeSafe::ExplicitConversion>;

		  public:

			using delta_t = typename std::make_signed<StoredType>::type;

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
			static delta_t worstDelta()
			{
				return std::numeric_limits<delta_t>::max();
			}

			self_t operator +(self_t fitness) const
			{
				return self_t{this->_id + fitness._id};
			}

			self_t operator +(delta_t delta) const
			{
				return self_t{this->_id + delta};
			}

			delta_t operator -(self_t fitness) const
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

	using Fitness = Private::FitnessImpl<Private::FitnessDummy, int>;
	using AdaptedFitness = Private::FitnessImpl<Private::AdaptedFitnessDummy, int>;

} //ns Algorithm

#endif //file guard
