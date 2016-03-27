#include "Cell.h"

namespace T3E
{
    Cell::Cell():
	tint_ (glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)),
	normalTint_(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)),
	brightTint_(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)),
	selected_(false),
	alternateMode_(false)	
    {
		//init colour ranges
		normalColourRange_[0] = glm::vec4(64.0f, 233.0f, 245.0f, 255.0f);
		normalColourRange_[1] = glm::vec4(64.0f, 88.0f, 245.0f, 255.0f);
		
		mutatedColourRange_[0] = glm::vec4(245.0f, 203.0f, 64.0f, 255.0f);
		mutatedColourRange_[1] = glm::vec4(245.0f, 145.0f, 64.0f, 255.0f);
		
		cancerousColourRange_[0] = glm::vec4(232.0f, 95.0f, 175.0f, 255.0f);
		cancerousColourRange_[1] = glm::vec4(164.0f, 95.0f, 232.0f, 255.0f);
	}

    Cell::~Cell()
    {
    }

	void Cell::init(CellState state, int deathChance)
	{		
		switch(state)
		{
		case CellState::STEM:
			spriteSheet_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/cellSheet.png", 0, 0, 1.0f/18, 1.0f/18, 18);
			state_ = state;
			normalTint_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // white
			alternateTint_ = glm::vec4(0.2, 0.8f, 0.2f, 1.0f);// green
			deathChance_ = 0;
			spriteSheet_.setSpeed(0.08);
			break;
		case CellState::NORMAL:
			spriteSheet_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/cellSheet.png", 0, 0, 1.0f/18, 1.0f/18, 18);
			state_ = state;
			setNormalTint(CellState::NORMAL);
			deathChance_ = deathChance;
			spriteSheet_.setSpeed(0.15);
			break;	
		case CellState::MUTATED:
			spriteSheet_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/cellSheet.png", 1.0f/18, 0, 1.0f/18, 1.0f/18, 18);
			state_ = state;
			setNormalTint(CellState::MUTATED);
			deathChance_ = deathChance;
			spriteSheet_.setSpeed(0.25);
			break;
		case CellState::CANCEROUS:
			spriteSheet_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/cellSheet.png", 2.0f/18, 0, 1.0f/18, 1.0f/18, 18);
			state_ = state;
			setNormalTint(CellState::CANCEROUS);
			deathChance_ = deathChance;
			spriteSheet_.setSpeed(0.4);
			break;
		default:
			SDL_Log("This log goes to the memory of that one f***ing bug, may he be remembered and never repeated");
			//mandatory treasure map with hint to solve this mysterious inscription - commit c263ddb77291b6dd94a99604cad51f4bc22b79cb
			break;
		}
		
		//init split time and timer
		splitTimer_ = 0;
		newSplitTime();
		
		//tints
		tint_ = normalTint_;
		brightTint_ = normalTint_*2.0f;
		brightAlternateTint_ = alternateTint_ * 2.0f;
	}
	
	bool Cell::update(float dTime)
	{
		if(state_ != CellState::ARRESTED)
		{
			spriteSheet_.Update(dTime);
		
			if((splitTimer_ += dTime) >= splitTime_)
			{
				//reset split timer
				splitTimer_ = 0.0f;
				//get a new random split time
				newSplitTime();
				return true;
			}
			return false;
		}
		return false;
	}
	
	void Cell::newSplitTime(int min, int max)
	{
		if(state_ == CellState::CANCEROUS)
		{
			splitTime_ = (min*16) + (rand() % (max*2));
		}
		else
			splitTime_ = min + (rand() % max);
	}
	
	void Cell::arrest()
	{
		state_ = CellState::ARRESTED;
		normalTint_ = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f); // grey
		tint_ = normalTint_;
		deathChance_ = 0;
		splitTimer_ = 0.0f;
	}
	
	void Cell::select()
	{
		selected_ = true;
		if(alternateMode_)
			tint_ = brightAlternateTint_;
		else
			tint_ = brightTint_;
	}
	
	void Cell::unselect()
	{
		selected_ = false;
		if(alternateMode_)
			tint_ = alternateTint_;
		else
			tint_ = normalTint_;
	}
	
	void Cell::toggleMode()
	{
		alternateMode_ = !alternateMode_;
		
		if(alternateMode_)
			tint_ = alternateTint_;
		else
			tint_ = normalTint_;
	}
	
	void Cell::incDeathChance(int dc)
	{
		deathChance_ += dc;
		//death test rolls 0 to 99, so need to cap
		if(deathChance_ > 99)
			deathChance_ = 99;
	}
	
 	void Cell::setNormalTint(CellState state)
	{
		int colourVariety = 1001;
		float rangePos =  rand()%colourVariety;//TODO: do we care about this?
		glm::vec4 diff;//max-min
		glm::vec4 result;
		switch(state)
		{
		case CellState::NORMAL:			
			diff = normalColourRange_[1] - normalColourRange_[0];
			result.x = normalColourRange_[0].x + diff.x/colourVariety*rangePos;	
			result.y = normalColourRange_[0].y + diff.y/colourVariety*rangePos;
			result.z = normalColourRange_[0].z + diff.z/colourVariety*rangePos;
			result.w = normalColourRange_[0].w + diff.w/colourVariety*rangePos;
			normalTint_ =  result/255.0f;
			break;
		case CellState::MUTATED:
			diff = mutatedColourRange_[1] - mutatedColourRange_[0];
			result.x = mutatedColourRange_[0].x + diff.x/colourVariety*rangePos;	
			result.y = mutatedColourRange_[0].y + diff.y/colourVariety*rangePos;
			result.z = mutatedColourRange_[0].z + diff.z/colourVariety*rangePos;
			result.w = mutatedColourRange_[0].w + diff.w/colourVariety*rangePos;
			normalTint_ =  result/255.0f;
			break;
		case CellState::CANCEROUS:
			diff = cancerousColourRange_[1] - cancerousColourRange_[0];
			result.x = cancerousColourRange_[0].x + diff.x/colourVariety*rangePos;	
			result.y = cancerousColourRange_[0].y + diff.y/colourVariety*rangePos;
			result.z = cancerousColourRange_[0].z + diff.z/colourVariety*rangePos;
			result.w = cancerousColourRange_[0].w + diff.w/colourVariety*rangePos;
			normalTint_ =  result/255.0f;
			break;
		default:
			SDL_Log("reached default case in Cell::setNormalTint()");
			break;
		}
	}
}
