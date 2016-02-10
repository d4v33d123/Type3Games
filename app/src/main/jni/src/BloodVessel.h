#ifndef BLOOD_VESSEL_H
#define BLOOD_VESSEL_H

#include "Type3Engine/Hex.h"
#include <cstdlib>//rand()

namespace T3E
{
	class BloodVessel: public Hex
	{
	public:		
		virtual void init(int column, int row, int gridColumns, int gridRows);//coords of central hex
		//getters
		glm::vec4 getTint(){return tint_;};
		float getRange(){return range_;};
		
	private:
		float range_;
		glm::vec4 tint_;//colour for shader
	};
}

#endif