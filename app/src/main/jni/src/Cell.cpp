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
			alternateTint_ = glm::vec4(1.0f, 0.7f, 0.7f, 1.0f);// pink
			deathChance_ = 0;
			spriteSheet_.setSpeed(0.08);
			break;
		case CellState::NORMAL:
			spriteSheet_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/cellSheet.png", 0, 0, 1.0f/18, 1.0f/18, 18);
			state_ = state;
			normalTint_ = glm::vec4(0.0f, 0.7f, 1.0f, 1.0f); // blue
			deathChance_ = deathChance;
			spriteSheet_.setSpeed(0.15);
			break;	
		case CellState::MUTATED:
			spriteSheet_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/cellSheet.png", 1.0f/18, 0, 1.0f/18, 1.0f/18, 18);
			state_ = state;
			normalTint_ = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f); // orange
			deathChance_ = deathChance;
			spriteSheet_.setSpeed(0.25);
			break;
		case CellState::CANCEROUS:
			spriteSheet_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/cellSheet.png", 2.0f/18, 0, 1.0f/18, 1.0f/18, 18);
			state_ = state;
			normalTint_ = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f); // magenta
			deathChance_ = 0;
			spriteSheet_.setSpeed(0.4);
			break;
		default:
			SDL_Log("This log goes to the memory of that one f***ing bug, may he be remembered and never repeated");
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
}
