#include "Cell.h"

namespace T3E
{
    Cell::Cell():
	tint_ (glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)),
	normalTint_(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)),
	brightTint_(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)),
	selected_(false)
    {
    }

    Cell::~Cell()
    {
    }

	void Cell::init(CellState state, int deathChance)
	{
		//init split time and timer
		splitTimer_ = 0;
		newSplitTime();
		
		spriteSheet_.init(-0.453f, -0.453f, 0.96f, 0.96f, "textures/cellSheet.png", 1.0f/18, 1.0f/18, 18);
		
		switch(state)
		{
		case CellState::STEM:
			state_ = state;
			normalTint_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // white
			deathChance_ = 0;
			spriteSheet_.setSpeed(0.08);
			break;
		case CellState::NORMAL:
			state_ = state;
			normalTint_ = glm::vec4(0.0f, 0.7f, 1.0f, 1.0f); // blue
			deathChance_ = deathChance;
			spriteSheet_.setSpeed(0.15);
			break;	
		case CellState::MUTATED:
			state_ = state;
			normalTint_ = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f); // orange
			deathChance_ = deathChance;
			spriteSheet_.setSpeed(0.25);
			break;
		case CellState::CANCEROUS:
			state_ = state;
			normalTint_ = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f); // magenta
			deathChance_ = 0;
			spriteSheet_.setSpeed(0.4);
			break;
		default:
			break;
		}

		tint_ = normalTint_;
		brightTint_ = normalTint_*2.0f;
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
		splitTime_ = min + (rand() % max);
	}
	
	void Cell::arrest()
	{
		state_ = CellState::ARRESTED;
		tint_ = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f); // grey
		deathChance_ = 0;
		splitTimer_ = 0.0f;
	}
	
	void Cell::select()
	{
		selected_ = true;
		tint_ = brightTint_;
	}
	
	void Cell::unselect()
	{
		selected_ = false;
		tint_ = normalTint_;
	}
}
