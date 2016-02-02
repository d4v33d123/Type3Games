#include "Cell.h"

namespace T3E
{
	Cell::Cell(int column, int row, int gridColumns, int gridRows)
	: type_(DEAD), tint_(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)), splitTimer_(0.0f), splitTime_(-1.0f), deathChance_(0)
	{		
		hex_.init(column, row, gridColumns, gridRows, HEX_SIZE);		
	}
	
	bool Cell::update(float dTime)
	{
		if((splitTimer_ += dTime) >= splitTime_)
		{
			splitTimer_ = 0.0f;
			return true;
		}
		return false;
	}
	
	void Cell::newSplitTime(int min, int max)
	{
		splitTime_ = min + (rand() % max);
	}
	
	void Cell::changeType(type t, int parentDeathChance)
	{
		switch(t)
		{
		case NORMAL:
			type_ = t;
			tint_ = glm::vec4(0.0f, 0.7f, 1.0f, 1.0f);//blue
			deathChance_ = parentDeathChance;
			break;
		case STEM:
			type_ = t;
			tint_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);//white
			deathChance_ = 0;
			break;
		case MUTATED:
			type_ = t;
			tint_ = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);//orange
			deathChance_ = parentDeathChance;
			break;
		case CANCEROUS:
			type_ = t;
			tint_ = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);//magenta
			deathChance_ = 0;
			break;
		case DEAD:
			type_ = t;
			break;
		case DYING:
			type_ = t;
			break;
		default:
			break;
		}
		//get a random split time
		newSplitTime();
	}
}