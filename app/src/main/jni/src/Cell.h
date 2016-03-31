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
	
	void makeGreen(){tint_ = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);} // for testing
	
	    Cell();
        virtual ~Cell();
		
		// The following static values are theoreticaly constant, but they aren't const
		// They are set by reading from the config file, So just dont change them with code
		// max and min split time
		static int MIN_ST;
		static int MAX_ST;

		// colour ranges (min and max)
		static glm::vec4 normalColourRange_[2];		
		static glm::vec4 mutatedColourRange_[2];
		static glm::vec4 cancerousColourRange_[2];
		
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
		void incDeathChance(int dc);
		
		// set state to arrested
		void arrest();
		
		// set/unset as currently selected
		void select();
		void unselect();
		
		//swaps between normal and alternate mode
		void toggleMode();

		//set random colour based on state
		void setNormalTint(CellState state); 
		
		//give a new tint. Sets current tint to normal!
		void hardcodeNormalTint(glm::vec4 normalTint);
		
		//play split animation
		void split(int neighbour);
		
		//don't wait for the split animation to end on creation
		void ignoreBirthDelay(){inCreation_ = false;};
		
		//draw the correct sprite
		void draw();
		
		//getters
		inline glm::vec4 getTint() { return tint_; }
		inline CellState getState() { return state_; }
		inline int getDeathChance() { return deathChance_; }
		inline bool isSelected() { return selected_; }
		inline bool isInAlternateMode() { return alternateMode_; }
		inline bool isDying(){return dying_;};
		inline bool isDead(){return dead_;};
		inline bool isSplitting(){return splitting_;};
		inline bool isInCreation(){return inCreation_;};
		inline float getSplitRotation(){return splitRotation_;};
		
		//setters
		void kill(){dying_ = true;};//play death animation
		void setDeathChance(int dc) { deathChance_ = dc; }
		
	private:
		float splitRotation_;//rotation for split animation sprite
		//spritesheets
		AnimatedSprite idleAnimation_;
		AnimatedSprite splitAnimation_;
		AnimatedSprite arrestAnimation_;
		AnimatedSprite deathAnimation_;
		
		glm::vec4 tint_; // current colour
		glm::vec4 normalTint_; // colour when non selected
		glm::vec4 brightTint_; // colour when selected
		glm::vec4 alternateTint_; // colour when alternate
		glm::vec4 brightAlternateTint_; // colour when alternate and selected

		CellState state_; // see CellState enum
		int deathChance_; // chance to die instead of splitting
		float splitTimer_; // time since last split; milliseconds //TODO: counting milliseconds with a float is probably a bad idea
		float splitTime_; // time to reach to start split; milliseconds
		
		bool selected_; // cell is the current selection?
		bool alternateMode_; // true when cell is in secondary mode (e.g. stem cell in spawn mode)
		bool dying_;//death animation is playing
		bool dead_;//cell needs to be deleted
		bool splitting_;//split animation playing
		bool inCreation_;//true while parent split animation is running
		bool fullyArrested_;//true = arrest animation finished playing
	};
}

#endif
