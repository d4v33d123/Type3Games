#ifndef BLOOD_VESSEL_H
#define BLOOD_VESSEL_H

#include "Hex.h"
#include <cstdlib>//rand()

namespace T3E
{
	class BloodVessel : public Node
	{
	public:
        BloodVessel();
        virtual ~BloodVessel();
		
		//getters
		float getRange() { return range_; };
		
	private:
		float range_;
	};
}

#endif
