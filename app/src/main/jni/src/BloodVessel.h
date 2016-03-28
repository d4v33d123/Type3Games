#ifndef BLOOD_VESSEL_H
#define BLOOD_VESSEL_H

#include "Hex.h"
#include "Type3Engine/AnimatedSprite.h"

namespace T3E
{
	class BloodVessel : public Node
	{
	public:
        BloodVessel();
        virtual ~BloodVessel();
		
		//animate sprite if forming
		void update(float dTime);
		
		//getters
		static void setRange( float range ) { range_ = range; }
		static float getRange() { return range_; };
		inline AnimatedSprite* getSprite() { return &spriteSheet_; }
		
	private:
		AnimatedSprite spriteSheet_;
		bool forming_;
		static float range_;
	};
}

#endif
