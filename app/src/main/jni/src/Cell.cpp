#include "Cell.h"

// Set default values, changed by the config.txt
int T3E::Cell::MIN_ST = 500;
int T3E::Cell::MAX_ST = 5000;

glm::vec4 T3E::Cell::normalColourRange_[2] = {
	glm::vec4( 0.0f, 0.0f, 255.0f, 255.0f ),
	glm::vec4( 0.0f, 0.0f, 255.0f, 255.0f )
};
glm::vec4 T3E::Cell::mutatedColourRange_[2] = {
	glm::vec4( 255.0f, 180.0f, 0.0f, 255.0f ),
	glm::vec4( 255.0f, 180.0f, 0.0f, 255.0f )
};
glm::vec4 T3E::Cell::cancerousColourRange_[2] = {
	glm::vec4( 255.0f, 0.0f, 255.0f, 255.0f ),
	glm::vec4( 255.0f, 0.0f, 255.0f, 255.0f )
};

namespace T3E
{
    Cell::Cell():
	tint_ (glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)),
	normalTint_(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)),
	brightTint_(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)),
	selected_(false),
	alternateMode_(false),
	dying_(false),
	dead_(false),
	splitting_(false),
	splitRotation_(0),
	inCreation_(true),
    showChange_(false),
    showingChange_(false),
	fullyArrested_(false),
    stemToStem_(false)
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
			idleAnimation_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/ssheet1.png", 6.0f/16, 0, 1.0f/16, 1.0f/16, 18, 21);
            splitAnimation_.init(-0.64, -0.465, 1.8f, 1.8f, "textures/ssheet3.png",  1.0f/2,  1.0f/2, 1.0f/12, 1.0f/12, 34, 6);
            stemToStemAnimation_.init(-0.64, -0.465, 1.8f, 1.8f, "textures/ssheet2.png",  5.0f/12,  0, 1.0f/12, 1.0f/12, 34, 6);
			state_ = state;
			normalTint_ = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // white
			alternateTint_ = glm::vec4(0.2, 0.8f, 0.2f, 1.0f);// green
			deathChance_ = 0;
			idleAnimation_.setSpeed(0.11);
            //tints
            tint_ = normalTint_;
            brightTint_ = normalTint_*1.5f;
            brightAlternateTint_ = alternateTint_ * 1.5f;
			break;
		case CellState::NORMAL:
			idleAnimation_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/ssheet1.png", 0, 0, 1.0f/16, 1.0f/16, 18, 18);
            splitAnimation_.init(-0.64, -0.465, 1.8f, 1.8f, "textures/ssheet3.png", 0, 0, 1.0f/12, 1.0f/12, 34, 6);
			state_ = state;
			setNormalTint(CellState::NORMAL);
			deathChance_ = deathChance;
			idleAnimation_.setSpeed(0.15);
            //tints
            tint_ = normalTint_;
            brightTint_ = normalTint_*2.0f;
            brightAlternateTint_ = alternateTint_ * 2.0f;
			break;	
		case CellState::MUTATED:
			idleAnimation_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/ssheet1.png", 2.0f/16, 0, 1.0f/16, 1.0f/16, 18, 18);
            splitAnimation_.init(-0.64, -0.465, 1.8f, 1.8f, "textures/ssheet3.png",  0,  1.0f/2, 1.0f/12, 1.0f/12, 34, 6);
            stateChangeAnimation_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/ssheet2.png", 2.0f/12, 0, 1.0f/12, 1.0f/12, 12, 12);
			state_ = state;
			setNormalTint(CellState::MUTATED);
			deathChance_ = deathChance;
			idleAnimation_.setSpeed(0.25);
            //tints
            tint_ = normalTint_;
            brightTint_ = normalTint_*2.0f;
            brightAlternateTint_ = alternateTint_ * 2.0f;
			break;
		case CellState::CANCEROUS:
			idleAnimation_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/ssheet1.png", 4.0f/16, 0, 1.0f/16, 1.0f/16, 18, 18);
            splitAnimation_.init(-0.64, -0.465, 1.8f, 1.8f, "textures/ssheet3.png",  1.0f/2,  0, 1.0f/12, 1.0f/12, 34, 6);
            stateChangeAnimation_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/ssheet2.png", 3.0f/12, 0, 1.0f/12, 1.0f/12, 14, 12);
			state_ = state;
			setNormalTint(CellState::CANCEROUS);
			deathChance_ = deathChance;
			idleAnimation_.setSpeed(0.4);
            //tints
            tint_ = normalTint_;
            brightTint_ = normalTint_*2.0f;
            brightAlternateTint_ = alternateTint_ * 2.0f;
			break;
		default:
			SDL_Log(" MISTAKES!!! Did you spawn a cell from an arrested one? no sprite is initialised so it will crash on the next glDrawArrays() call");
			break;
		}
		
		//init split time and timer
		splitTimer_ = 0;
		newSplitTime();
        
        //note: so much stuff that's useless depending on the type of cell... and not just anim sprite sheets... could rework into multiple classes.
		deathAnimation_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/ssheet0.png", 1.0f/2, 0, 1.0f/10, 1.0f/10, 22, 5);
        arrestAnimation_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/ssheet0.png", 0, 0, 1.0f/8, 1.0f/8, 13, 4);
        arrestIdleAnimation_.init(-0.43f, -0.43f, 0.86f, 0.86f, "textures/ssheet2.png", 0, 0, 1.0f/12, 1.0f/12, 14, 12);
        arrestIdleAnimation_.setSpeed(0.09);
	}
	
	bool Cell::update(float dTime)
	{
		// TODO: incorporate some of these into the cell state enum and
		// use a switch case instead, povided we can only be in one state at a time

		if(dying_)
		{
			dead_ = deathAnimation_.Update(dTime);
		}
		else if(state_ == CellState::ARRESTED)
		{
			if(!fullyArrested_)
			{
				fullyArrested_ = arrestAnimation_.Update(dTime);
				if(fullyArrested_)//here because we want to draw the last frame and not repeat this every subsequent update
				{
					normalTint_ = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f); // grey
					tint_ = normalTint_;
				}		
			}
            else
            {
                arrestIdleAnimation_.Update(dTime);
            }
		}		
		else if( inCreation_ )//don't update if parent split animation is playing
		{
            if(showingChange_)
            {
                if(stateChangeAnimation_.Update(dTime))
                {
                    inCreation_ = false;
                }
            }
            else
            {
                //sync a split animation with the parent. when it's done we can start drawing and updating normally
                if(splitAnimation_.Update(dTime))
                {
                    if(showChange_)                    
                        showingChange_ = true;
                    else
                        inCreation_ = false;
                }
            }
		}			
		else if( splitting_ )
		{
            if(state_ == CellState::STEM && stemToStem_)
            {
                if(stemToStemAnimation_.Update(dTime))
                {
                    idleAnimation_.refresh();
                    idleAnimation_.Update(0);//needRefresh_ will be set so dTime is irrelevant
                    splitting_ = false;
                    stemToStem_ = false;
                }
            }
			else if(splitAnimation_.Update(dTime))
			{
				idleAnimation_.refresh();
				idleAnimation_.Update(0);//needRefresh_ will be set so dTime is irrelevant
				splitting_ = false;
			}			
		}
		else
		{
			// The cell is in currently idling
			idleAnimation_.Update(dTime);
	
			if((splitTimer_ += dTime) >= splitTime_)
			{
				//reset split timer
				splitTimer_ = 0.0f;
				//get a new random split time
				newSplitTime();
				return true;
			}
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
		float rangePos =  rand()%colourVariety;
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
	
	void Cell::hardcodeNormalTint(glm::vec4 normalTint)
	{
		normalTint_ = normalTint;
		tint_ = normalTint_;
		brightTint_ = normalTint_*2.0f;
	}
	
	void Cell::split(int neighbour)
	{
		splitting_ = true;
		
		splitAnimation_.refresh();
		//needRefresh_ will be set so dTime is irrelevant
		splitAnimation_.Update(0);
		
		//relies on ordering of Grid::getNeighbours()
		switch(neighbour)
		{
		case 0://left
			splitRotation_ = 120;
			break;
		case 1://top left
			splitRotation_ = 60;
			break;
		case 2://bottom left
			splitRotation_ = 180;
			break;
		case 3://top right
			splitRotation_ = 0;
			break;
		case 4://bottom right
			splitRotation_ = 240;
			break;
		case 5://right
			splitRotation_ = 300;
			break;
		default:
			SDL_Log("Cell::split() reached default case");
			break;			
		}
	}
	
	void Cell::draw()
	{
		if(inCreation_)
		{
			if(showingChange_)
                stateChangeAnimation_.draw();
		}
        else
        {
            if(dying_)
				deathAnimation_.draw();
			else if(state_ == CellState::ARRESTED)
            {
                if(fullyArrested_)
                    arrestIdleAnimation_.draw();
                else
                    arrestAnimation_.draw();
            }				
			else if(splitting_)
            {
                if(state_ == CellState::STEM && stemToStem_)
                    stemToStemAnimation_.draw();
                else
                    splitAnimation_.draw();
            }						
			else
				idleAnimation_.draw();	
        }
	}
}
