//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _algorithm_isolution_included
#define _algorithm_isolution_included

#include "fitness.h"
#include <boost/property_tree/ptree_fwd.hpp>

namespace Algorithm {

	class ISolution {

	  public:

		virtual ISolution *Clone() const = 0;		//replaces copy constructor
		virtual void CopyTo(ISolution *) const = 0;	//replaces assignment operator

		virtual bool IsEqual(const ISolution *) const = 0;

		virtual bool Normalize() = 0; //ensure the solution is in a valid state to start an algorithm, should return true if changes were made
		virtual Fitness GetFitness() = 0; //returns the quality ('fitness') of this solution
		virtual bool IsFeasible() = 0;
		virtual void MarkDirty() = 0; //requests recalculation of fitness

		virtual void Load(const boost::property_tree::ptree &) = 0;
		virtual void Save(boost::property_tree::ptree &) const = 0;

		virtual ~ISolution() {};
		ISolution &operator =(const ISolution &) = delete; //prevent non-polymorphic copying
	};

} //ns Algorithm

#endif