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
	
	    Cell();
        virtual ~Cell();
		
		/** @name minimum and maximum split times
		* The following static values are theoreticaly constant, but they aren't const
		* They are set by reading from the config file, So just dont change them with code, please
		* max and min split time
		*/
        ///@{
		static int MIN_ST;
		static int MAX_ST;
		///@}

        /** @name Cell colour ranges
		* These colour values are shared across all cells.
		* Cells will then choose a random colour between these two values depenting on their state
		* @note These are given inital values, but they are likely to be overwritten by whats in the config.txt anyway
        */
        ///@{
		static glm::vec4 normalColourRange_[2];		
		static glm::vec4 mutatedColourRange_[2];
		static glm::vec4 cancerousColourRange_[2];
		///@}
		
		/**
		* @param [in] The state the cell should be in after initalisation
		* @param [in] The inital death chance of the cell
		* @see T3E::CellState
		*/
		void init(CellState state, int deathChance);
		
		/**
		// @param [in] deltaTime time since last frame was rendered
		// @return true if time to split, false otherwise
		// increase timer
		*/
		bool update(float dTime);
		
		// TODO: should this be private instaed?
		// TODO: `new` impiles memory allocation, should rename
		/**
		* @param [in] minimum new split timer value
		* @param [in] maximum new split timer value
		* Recalculates the spit time of the cell
		*/
		void newSplitTime(int min = MIN_ST, int max = MAX_ST);
	
		/***
		* @param [in] dc Amount to add to current death chance
		*/		
		void incDeathChance(int dc);
		
		/***
		* Set state to arrested and reset timers
		*/
		void arrest();
		
		/**
		* set/unset as currently selected
		*/
		///@{
		void select();
		void unselect();
		///@}

		//swaps between normal and alternate mode
		/**
		* Switches between normal and alternate mode.
		* Only relevant for stem cells, alternate mode is when they can split
		*/
		void toggleMode();
		
		/**
		* @param [in] tint Sets the current tint to a Vec4 XYZW = RGBA
		* Sets current tint to normal!
		*/
		void hardcodeNormalTint(glm::vec4 normalTint);
		
		/**
		* @param [in] neighbour Which of the 6 neighbours to split into
		* play split animation rotated to towards the given neighbour
		*/
		void split(int neighbour);
		
		/**
		* Calls draw on the correct sprite depending on the cells state
		*/
		void draw();


        /** @name Getters */
        ///@{
		inline glm::vec4 getTint() { return tint_; }
		inline CellState getState() { return state_; }
		inline int getDeathChance() { return deathChance_; }
		inline bool isSelected() { return selected_; }
		inline bool isInAlternateMode() { return alternateMode_; }
		inline bool isDying() { return dying_; }
		inline bool isDead() { return dead_; }
		inline bool isSplitting() { return splitting_; }
		inline bool isInCreation() { return inCreation_; }
		inline float getSplitRotation() { return splitRotation_; }
		///@}
		
        /** @name Setters */
        ///@{
		/**
		* Takes in a CellState and sets the sprites colour to a value aproprite for that state
		*
		* This is what gives cells their slight colour variation even within cells of the same type
		* @param [in] state The state that this cells colour should reflect
		*/
		///@{
        void stemToStemOn() { stemToStem_ = true; }
        void showChangeOn() { showChange_ = true; }
		void setNormalTint( CellState state );							///< Set the colour based on the given cell state
		void kill() { dying_ = true; } 									///< play death animation
		void setDeathChance(int dc) { deathChance_ = dc; }				///< Sets the cells death chance @warning does not clamp range
		void ignoreBirthDelay() { inCreation_ = false; } 				///< don't wait for the split animation to end on creation
		void makeGreen() { tint_ = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); }	///< for testing mainly
		///@}
		
	private:
		float splitRotation_; // rotation for split animation sprite

		AnimatedSprite idleAnimation_;
		AnimatedSprite splitAnimation_;
		AnimatedSprite arrestAnimation_;
		AnimatedSprite deathAnimation_;        
        AnimatedSprite stemToStemAnimation_;         
        AnimatedSprite arrestIdleAnimation_;
        AnimatedSprite stateChangeAnimation_;
        
		glm::vec4 tint_; // current colour
		glm::vec4 normalTint_; // colour when non selected
		glm::vec4 brightTint_; // colour when selected
		glm::vec4 alternateTint_; // colour when alternate
		glm::vec4 brightAlternateTint_; // colour when alternate and selected

		CellState state_; // see CellState enum
		int deathChance_; // chance to die instead of splitting
		float splitTimer_; // time since last split; milliseconds //TODO: counting milliseconds with a float is probably a bad idea
		float splitTime_; // time to reach to start split; milliseconds
		
		// TODO: Some of these should probably be incorporated into the CellState enum
		bool selected_; // cell is the current selection?
		bool alternateMode_; // true when cell is in secondary mode (e.g. stem cell in spawn mode)
        bool stemToStem_;//a stem cell is being (manually) split into another stem cell
        bool showChange_;//cell should play state change animation before idling
        bool showingChange_;//currently doing ^^^
		bool dying_; // death animation is playing
		bool dead_; // cell needs to be deleted
		bool splitting_; // split animation playing
		bool inCreation_; // true while parent split animation is running
		bool fullyArrested_; // true = arrest animation finished playing
	};
}

#endif
