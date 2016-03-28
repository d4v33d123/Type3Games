#include "BloodVessel.h"
namespace T3E
{
	float T3E::BloodVessel::range_ = 5;

	BloodVessel::BloodVessel()
	:forming_(true)
	{
		spriteSheet_.init(-1.5f, -1.5f, 3.0f, 3.0f, "textures/bloodVessel.png", 0, 0, 1.0f/5, 1.0f/5, 23, 5);
	}
	
	BloodVessel::~BloodVessel()
    {
    }
	
	void BloodVessel::update(float dTime)
	{
		if(forming_)
			forming_ = !spriteSheet_.Update(dTime);
	}
}
