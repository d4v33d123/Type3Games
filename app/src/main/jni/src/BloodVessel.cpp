#include "BloodVessel.h"
namespace T3E
{
	void BloodVessel::init(int column, int row, int gridColumns, int gridRows)
	{	
		//init hex coorinates
		Hex::init(column, row, gridColumns, gridRows);
		//set type to bv
		type_ = BLOOD_VESSEL;
		//Set tint
		tint_ = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		//range is how many surrounding hexes are affected + 1
		range_= 3;
	}
}