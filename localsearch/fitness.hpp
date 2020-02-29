//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_fitness_included_
#define _algorithm_fitness_included_

#include <ctoolhu/typesafe/id.hpp>
#include <cassert>
#include <limits>
#include <ostream>
#include <stdexcept>

namespace Algorithm {

	namespace Private {

		template <class RequestingObject, typename StoredType>
		class FitnessImpl : public Ctoolhu::TypeSafe::Id<RequestingObject, StoredType, Ctoolhu::TypeSafe::ExplicitConversion> {

			using base_t = Ctoolhu::TypeSafe::Id<RequestingObject, StoredType, Ctoolhu::TypeSafe::ExplicitConversion>;

		  public:

			using delta_t = typename std::make_signed<StoredType>::type;

			FitnessImpl() noexcept : base_t(0) {}

			explicit FitnessImpl(StoredType id) noexcept : base_t(id)
			{
				_verify();
			}

			static FitnessImpl worst() noexcept
			{
				return FitnessImpl{std::numeric_limits<StoredType>::max()};
			}

			//worst value of delta (never expected to be reached by an actual step)
			static delta_t worstDelta() noexcept
			{
				return std::numeric_limits<delta_t>::max();
			}

			bool isZero() const noexcept
			{
				return this->_id == 0;
			}

			FitnessImpl operator +(FitnessImpl fitness) const noexcept
			{
				return FitnessImpl{this->_id + fitness._id};
			}

			FitnessImpl operator +(delta_t delta) const noexcept
			{
				return FitnessImpl{this->_id + delta};
			}

			delta_t operator -(FitnessImpl fitness) const noexcept
			{
				return this->_id - fitness._id;
			}

			void operator +=(FitnessImpl fitness) noexcept
			{
				this->_id += fitness._id;
			}

			void operator +=(int penalty) noexcept
			{
				this->_id += penalty;
				_verify();
			}

			void operator -=(FitnessImpl fitness) noexcept
			{
				this->_id -= fitness._id;
				_verify();
			}

			void operator -=(int penalty) noexcept
			{
				this->_id -= penalty;
				_verify();
			}

			friend std::ostream &operator<<(std::ostream &out, const FitnessImpl &fitness)
			{
				return out << fitness._id;
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
