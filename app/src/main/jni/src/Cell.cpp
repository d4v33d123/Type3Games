#include "Cell.h"

namespace T3E
{
	void Cell::init(int column, int row, int gridColumns, int gridRows)
	{
		//init hex coorinates
		Hex::init(column, row, gridColumns, gridRows);
		tint_ = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		splitTimer_ = 0.0f;
		splitTime_ = -1.0f;
		deathChance_ = 0;
		//init type to dead
		type_ = DEAD_CELL;
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
	
	void Cell::setType(type t, int parentDeathChance)
	{
		switch(t)
		{
		case STEM_CELL:
			type_ = t;
			tint_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);//white
			deathChance_ = 0;
			break;
		case NORMAL_CELL:
			type_ = t;
			tint_ = glm::vec4(0.0f, 0.7f, 1.0f, 1.0f);//blue
			deathChance_ = parentDeathChance;
			break;	
		case MUTATED_CELL:
			type_ = t;
			tint_ = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);//orange
			deathChance_ = parentDeathChance;
			break;
		case CANCEROUS_CELL:
			type_ = t;
			tint_ = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);//magenta
			deathChance_ = 0;
			break;
		default:
			type_ = t;
			break;
		}
		//get a random split time
		newSplitTime();
	}
}