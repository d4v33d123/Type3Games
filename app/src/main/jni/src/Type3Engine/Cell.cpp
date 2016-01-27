#include "Cell.h"

#include "SDL.h"//////SDL LOG testing purpose

namespace T3E
{
	Cell::Cell(float column, float row, type t)
	: c_(column), r_(row), type_(t), tint_(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)), hexSize_(0.54f), splitTimer_(0.0f), deathChance_(100), divTime(1000)
	{		
		layout_ = glm::mat2(sqrt(3.0f), sqrt(3.0f) / 2.0f, 0.0f, 3.0f / 2.0f);
        layoutInverse_= glm::mat2(sqrt(3.0f) / 3.0f, -1.0f / 3.0f, 0.0f, 2.0f / 3.0f);
			
		x_ = (layout_[0][0] * c_ + layout_[0][1] * r_) * hexSize_;
		y_ = (layout_[1][0] * c_ + layout_[1][1] * r_) * hexSize_;
		
		//condensed hardcoded grid size yolo
		int size = 30;//number of rows and columns in the grid
		
		neighbors.resize(6);
		
		neighbors[0].col = c_;
		neighbors[0].row = r_-1;
		neighbors[1].col = c_+1;
		neighbors[1].row = r_-1;
		neighbors[2].col = c_+1;
		neighbors[2].row = r_;
		neighbors[3].col = c_;
		neighbors[3].row = r_+1;
		neighbors[4].col = c_-1;
		neighbors[4].row = r_+1;
		neighbors[5].col = c_-1;
		neighbors[5].row = r_;
		
		//set invalid positions to -1
		for(int i = 0; i < 6; ++i)
		{
			if((neighbors[i].col >= size) || (neighbors[i].col < 0) || (neighbors[i].row >= size) || (neighbors[i].row < 0))
			{	neighbors[i].col = -1;
				neighbors[i].row = -1;
			}
		}
/* 		
		if(type_ == STEM)
		{
			deathChance = 0;
			tint_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		
		if(type_ == MUTATED)
		{
			deathChance = 0;
			tint_ = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
		}
		
		if(type_ == NORMAL)
		{
			deathChance = 100;
			tint_ = glm::vec4(0.0f, 0.7f, 1.0f, 1.0f);
		}
		
		if(type_ == DEAD)
		{
			deathChance = 0;
			tint_ = glm::vec4(1.0f, 1.0f, .0f, 1.0f);
		}
		 */
	}
	
	bool Cell::update(float dTime)
	{
		if((splitTimer_ += dTime) > divTime)
		{
			splitTimer_ = 0.0f;
			return true;
		}
		return false;
	}
}