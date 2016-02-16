#ifndef BLOOD_VESSEL_H
#define BLOOD_VESSEL_H

#include "Hex.h"
#include <cstdlib>//rand()

namespace T3E
{
	class BloodVessel : public Node
	{
	public:
        BloodVessel() {}
        virtual ~BloodVessel() {}

		//virtual void init(int column, int row, int gridColumns, int gridRows);//coords of central hex
		//getters
		//glm::vec4 getTint() { return tint_; }
		//float getRange() { return range_; }
		
	private:
		//float range_;
		//glm::vec4 tint_;//colour for shader
	};
}

#endif
