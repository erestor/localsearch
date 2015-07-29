//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_fitness_included_
#define _algorithm_fitness_included_

#include <ctoolhu/typesafe/id.hpp>
#include <type_traits>
#include <stdexcept>

namespace Algorithm {

	namespace Private {

		template <class RequestingObject, typename StoredType>
		class FitnessImpl : public Ctoolhu::TypeSafe::Id<RequestingObject, StoredType, Ctoolhu::TypeSafe::ExplicitConversion> {

			typedef FitnessImpl<RequestingObject, StoredType> self_type;
			typedef Ctoolhu::TypeSafe::Id<RequestingObject, StoredType, Ctoolhu::TypeSafe::ExplicitConversion> base_type;

		  public:

			typedef typename std::make_signed<StoredType>::type delta_type;

			FitnessImpl() : base_type(0) {}
			explicit FitnessImpl(StoredType id) : base_type(id)
			{
				_Verify();
			}

			self_type operator +(const self_type &fitness) const
			{
				return self_type(this->_id + fitness._id);
			}

			self_type operator +(delta_type delta) const
			{
				return self_type(this->_id + delta);
			}

			delta_type operator -(const self_type &fitness) const
			{
				return this->_id - fitness._id;
			}

			void operator +=(const self_type &fitness)
			{
				this->_id += fitness._id;
				_Verify();
			}

			void operator +=(int penalty)
			{
				this->_id += penalty;
				_Verify();
			}

			void operator -=(int penalty)
			{
				this->_id -= penalty;
				_Verify();
			}

		  private:

			void _Verify()
			{
				if (this->_id < 0)
					throw std::logic_error("Algorithm::Private::FitnessImpl::_Verify: Fitness can't be negative");
			}
		};

		class FitnessDummy;
		class AdaptedFitnessDummy;

	} //ns Private

	typedef Private::FitnessImpl<Private::FitnessDummy, int> Fitness;
	typedef Private::FitnessImpl<Private::AdaptedFitnessDummy, int> AdaptedFitness;

} //ns Algorithm

#endif //file guard
