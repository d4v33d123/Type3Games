#include "Hex.h"

namespace T3E
{
	void Hex::init(int column, int row, int gridColumns, int gridRows)
	{
		c_ = column;
		r_ = row;
		hexSize_ = HEX_SIZE;
			
		//calculate world x and y coordinates of hex centre
		//pointy top layout
		layout_ = glm::mat2(sqrt(3.0f), sqrt(3.0f) / 2.0f, 0.0f, 3.0f / 2.0f);
        layoutInverse_= glm::mat2(sqrt(3.0f) / 3.0f, -1.0f / 3.0f, 0.0f, 2.0f / 3.0f);
		x_ = (layout_[0][0] * c_ + layout_[0][1] * r_) * hexSize_;
		y_ = (layout_[1][0] * c_ + layout_[1][1] * r_) * hexSize_;

		//calculate neighbor's grid coords
		neighbors_.resize(6);		
		neighbors_[0].col = c_;
		neighbors_[0].row = r_-1;
		neighbors_[1].col = c_+1;
		neighbors_[1].row = r_-1;
		neighbors_[2].col = c_+1;
		neighbors_[2].row = r_;
		neighbors_[3].col = c_;
		neighbors_[3].row = r_+1;
		neighbors_[4].col = c_-1;
		neighbors_[4].row = r_+1;
		neighbors_[5].col = c_-1;
		neighbors_[5].row = r_;
		//set out of bounds positions to -1
		for(int i = 0; i < 6; ++i)
		{
			if((neighbors_[i].col >= gridColumns) || (neighbors_[i].col < 0) || (neighbors_[i].row >= gridRows) || (neighbors_[i].row < 0))
			{
				neighbors_[i].col = -1;
				neighbors_[i].row = -1;
			}
		}
	}
	
	bool Hex::inRange(float c, float r, float range)
	{
		int dist = (std::abs(c - c_) + std::abs(r - r_) + std::abs((-c-r) - (-c_-r_))) / 2;
		if(dist > range)
			return false;
		return true;
	}
}