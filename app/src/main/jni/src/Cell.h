#ifndef CELL_H
#define CELL_H

#include "Hex.h"
#include "SDL.h"
#include "Type3Engine/AnimatedSprite.h"
#include <cstdlib>//rand()

namespace T3E
{	
	enum class CellState { STEM, NORMAL, MUTATED, CANCEROUS, ARRESTED };
	
	class Cell: public Node
	{		
	public:
	
	void makeGreen(){tint_ = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);};//for testing
	
	    Cell();
        virtual ~Cell();
		
		//max and min split time
		static const int MIN_ST = 500;
		static const int MAX_ST = 5000;
		
		// [in] see CellState enum
		void init(CellState state, int deathChance);
		
		// [in] time since last frame was rendered
		// [ret] true if time to split, false otherwise
		// increase timer
		bool update(float dTime);
		
		// [in] minimum new split timer value
		// [in] maximum new split timer value
		// get a new random div time <= max
		void newSplitTime(int min = MIN_ST, int max = MAX_ST);
	
		// [in] amount to add to current death chance
		void incDeathChance(int dc){deathChance_ += dc;};
		
		// set state to arrested
		void arrest();
		
		// set/unset as currently selected
		void select();
		void unselect();
		
		//getters
		glm::vec4 getTint(){return tint_;};
		CellState getState(){return state_;};
		int getDeathChance(){return deathChance_;};
		bool isSelected(){return selected_;};
		AnimatedSprite* getSprite(){return &spriteSheet_;};
		
		//setters
		void setDeathChance(int dc){deathChance_ = dc;};
		
	private:
		AnimatedSprite spriteSheet_;
		CellState state_;//see CellState enum
		glm::vec4 tint_;//current colour
		glm::vec4 normalTint_;//colour when non selected
		glm::vec4 brightTint_;//colour when selected
		bool selected_;//cell is the current selection?
		int deathChance_;//chance to die instead of splitting
		float splitTimer_;//time since last split; milliseconds
		float splitTime_;//time to reach to start split; milliseconds
	};
}

#endif
