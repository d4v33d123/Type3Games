#ifndef GRID_H
#define GRID_H

#include <array>
#include <vector>
#include "Hex.h"
#include "Cell.h"
#include "BloodVessel.h"
#include "SDL.h"//log

enum class InteractionMode {NORMAL, BVCREATION, KILLMODE};

//TODO: some functions are redundant or check unnecessary things
namespace T3E
{
    class Grid
    {
    public:
        Grid();
        ~Grid();

        /**
        * Get the pointer to a node at the given row / column
        *
        * @param [in] row
        * @param [in] column
        * @param [out] node at given row column if found
        * @return true if given row/col lies on board, false otherwise
        */
        bool getNode( int row, int col, Node** node );
		
        /**
        * Get a pointer to the hex at a givent row / column
        *
        * If you want the hex's node you could just use Grid::getNode()
        * Otherwise you could use Hex::getNode() by calling hexPtr->getNode() on the returned pointer
		* @param [in] row
        * @param [in] column
        * @param [out] hex at given row column if found
        * @return True if given row/col lies on board, false otherwise
        */
		bool getHex( int row, int col, Hex** hex );

        /**
        * Get an array of pointers to the 6 neighbours of the given hex
        * 
        * If a neighbour is uninitalised NodeType::EMPTY
        * If the node does not exist (lies off the board) then it is set to nullptr
        * @param [in] row of central node
        * @param [in] column of central node
        * @param [out] pointer to Hex*[6], array must have 6 elements!
        * @return false if the givn row, col does not exist
        */
        bool getNeighbours( int row, int col, Hex** neighbours );

        /**
        * Create a new cell with a givent state at the row / col, and optionaly get a pointer to that cell
        *
        * @param [in] row to create cell
        * @param [in] column to create cell
		* @param [in] cell state (see CellState enum in Cell.h)
		* @param [in] death chance; this is ignored if the new cell is stem or cancerous
        * @param [out] pointer A pointer to the newly created cell, the caller can pass nullptr if they do not need this
        * @return true if given row/col lies on board & cell created, false otherwise
        */
        bool newCell( int row, int col, CellState state, int deathChance, Cell** createdCell );

        /**
        * Create a new blood vesse at the given row / col, optonaly pet a pointer to that BloodVessel
        *
        * @param [in] row The row at which to create the cell
        * @param [in] col The column at which to create the cell
        * @param [out] pointer The newly created blood vessel, nulptr if the caller doesn't care
        * @return true if the givenrow/col lie on the board & bloodVessel created, false otherwise
        */
        bool newBloodVessel( int row, int col, BloodVessel** createdBloodVessel );

        /**
        * Use to clear the contents of a hex at the given row / col
        * If the given hex is part of a blood vessel, this will remove the entire blood vesel
        *
        * @param [in] row The row of node to be deleted
        * @param [in] col The column of node to be deleted
        */
        void setEmpty( int row, int col );

        /**
        * @param [in] row The row of the node to check
        * @param [in] col The column of the node to check
        * @return Returns false if the hex doesn't exist, or exists and is not empty.
        * Returns true if it exists and is empty
        */
        inline bool isEmpty( int row, int col ) { if( !hexExists( row, col ) ) return false; return grid_[row*CHUNK_WIDTH+col].getType() == NodeType::EMPTY; }

        /**
        * @param the index of the cell to get from the internal vector
        * @warning Does not do bounds checking!
        * @see numCells()
        */
        inline Hex* getCell( int i ) const { return cells_[i]; }

        /**
        * @return The number of cells in the internal vector
        * This includes all cell types: healthy, cancerous, mutated ...
        */
        inline size_t numCells() const { return cells_.size(); }

        /**
        * @param the index of the BloodVessel to get from the intenal vector
        * @warning Does not do bunds checking!
        * @see numBloodVessels()
        */
        inline Hex* getBloodVessel( int i ) const { return bloodVessels_[i]; }

        /**
        * @return The number of blood vessels in the internal vector
        * Only counts each BloodVessel, NOT the numbef of cells in the blood vessels
        */
        inline size_t numBloodVessels() const { return bloodVessels_.size(); }

        /**
		* @param [in] float delta time since last frame was rendered
		* @param [in] finger position (row column), to avoid killing cell that is being hovered
		* @return true if a selected cell died
		* TODO: this return is quite unrelated to the function,
		* 		 but I couldn't think of a better way to unset the cellSelected_ flag in MainGame when this happens
		* call update methods of grid elements, spawn/kill cells according to sim rules
		*/
		bool update( float dTime, SDL_Point fingerRowCol );
		
		//TODO: use hexexist checks in getdistance and inrange ? or not since we check before anyway since it's only used internally?
		
		/**
		* Checks if two cells are in range, adjacent cells have a range of 1
		*
		* @param [in] row Row of the first hex
		* @param [in] col Column of the first hex
		* @param [in] row Row of the second hex
		* @param [in] col Column of the second hex
		* @param [in] maximum range in number of hexes 
		* @return true if the distance between the two hexes is less than or equal to range
		*/
		bool inRange( int rowA, int colA ,int rowB, int colB, int range );
		
		/**
		* Checks the distance between two cells, adjacent cells have a distance of 1
		*
		* @param [in] row The row of the first hex
		* @param [in] col The column of the first hex
		* @param [in] row The row of the second hex
		* @param [in] col The column of the second hex
		* @return The distance, in number of hexes, between the two given hex coords
		*/
		int getDistance( int rowA, int colA ,int rowB, int colB );
		
		/**
		* @param [in] row of cell to arrest
		* @param [in] column of cell to arrest
		* @param [out] True if the arrested cell was selected
		* @return false if cell doesn't exist or is not CellState::NORMAL, true if success
		*/
		bool arrestCell( int row, int col, bool* cellSelected );
		
		/**
		* @param [in] row of cell to select
		* @param [in] column of cell to select
		* @return false if cell doesn't exist or is not CellState::NORMAL/STEM, true if success
		*/
		bool selectCell( int row, int col );
		
		/**
		* @param [in] row of cell to unselect
		* @param [in] column of cell to unselect
		* @return false if cell doesn't exist or is not CellState::NORMAL/STEM, true if success
		*/
		bool unselectCell( int row, int col );
		
		/**
		* @param [in] row of currently selected cell
		* @param [in] column of currently selected cell
		* @param [in] row of touched hex
		* @param [in] column of touched hex
		* @return true if new cell is created, false otherwise
		* calles when player tries to spawn cell manually
		*/
		bool spawnCell( int selRow, int selCol, int touchRow, int touchCol );
		
		/**
		* @param [in] row of cell to delete
		* @param [in] column of cell to delete
		* @return true if new cell is deleted, false otherwise
		* calles when player tries to kill cell manually
		*/
		bool killCell( int row, int col );
		
		/**
		* [ret] size of one of the grid's dimensions
		* 		assuming height = width
		*/
		int getSize() { return CHUNK_WIDTH; }
		
		/**
		* TODO: kinda meh designed function?...
		* @param [in] row of hex to be queried
		* @param [in] column of hex to be queried
		* @return vec3 where x and y are world coords of hex
		*		z = 0 if hex is in blood vessel range ,1 if in large range, 2 otherwise
		*		w = lerp factor for colour
		*		x and y will be -1 if hex doesnt exist(our grid is all in positive coords so that works for now)
		*/
		glm::vec4 getHexDrawInfo(int row, int col, bool cellSelected, glm::vec2 selectedPos, InteractionMode interactionMode_);
		
		/**
		* @param [in] row of currently selected cell
		* @param [in] column of currently selected cell
		* @param [in] row of touched hex
		* @param [in] column of touched hex
		* @return true if stem was moved false otherwise
		*/
		bool moveStemCell(int selRow, int selCol, int touchRow, int touchCol);
		
		/**
		* @param [in] row of cell to test
		* @param [in] column of cell to test
		* @return false if cell doesn't exist or is not CellState::STEM, true if success
		* sets stem cell in spawn mode
		*/
		bool setStemToSpawnMode(int row, int col);
		
		/**
		* Tries to grow a blood vessel at the given coords
		*
		* newBloodVessel() is the function that actualy creates the blood vessel + allocates memory
		* @param [in] row to test
		* @param [in] column to test
		* @param [out] selected cell died, needed to notify update loop and remove grid huighlight
		* @return true if a bv was created, false otherwise
		* try to create a blood vessel at the specified position
		*/
		bool growBloodVesselAt( int row, int col, bool* selectedCellDied);
		
		/**
		* @return true if to play blood vessel sound
		*/
		inline bool playVessel() { return playVessel_; }
		
		/**
		* Resets the blood vessel sound effect
		*/
		inline void resetPlayVessel() { playVessel_ = false; }
				
		/**
		* @return Returns the current score
		*/
		inline int getHighScore() { return high_score_; }
		/**
		* @return Returns the current currency
		*/
		inline int getCurrency() { return currency_; }
		/**
		* Sets the score and currency simultaniously
		*/
		inline void setScore( int score ) { currency_ = high_score_ = score; }
		/**
		* Adds positive vales to both currency and score, subtracts -vs values from currency
		*
		* @param value to add to the score, can be +ve or -ve
		*/
		void addScore( int score );

		/**
		* try to create a blood vessel spawn point at the specified position
		*
		* @param [in] row to test
		* @param [in] column to test
		* @return true if success
		*/
		bool setBvSpawn( int row, int col );
		
		/**
		* @warning Does not do range checking!
		* @see numBvSpawns()
		* @return The world coordinates of the BloodVessel spawn marker of index i
		*/
		glm::vec2 getBvSpawnCoords( int i );
		
		/**
		* @return The number of BloodVessel spawn points on the board
		*/
		int numBvSpawns() { return bvSpawnPoints_.size(); }

		/** @name probability setters
		* Group of functions set the random chances for various events.
		*/
		///@{
		void setChanceOfMutation( int c ) 			{ chanceOfMutation_ = clampPercnt( c ); }
		void setChanceOfCancer( int c ) 			{ chanceOfCancer_ = clampPercnt( c ); }
		void setCancerDeathChance( int c ) 			{ cancerDeathChance_ = clampPercnt( c ); }
		void setAdjBloodvesselDeathChance( int c ) 	{ adjacentBloodvesselDeathChance_ = clampPercnt( c ); }
		void setFarBloodvesselDeathChance( int c ) 	{ farBloodvesselDeathChance_ = clampPercnt( c ); }
		void setParentDeathChanceIncrease( int c ) 	{ parentDeathChanceIncrease_ = clampPercnt( c ); }
		void setChildDeathChanceIncrease( int c ) 	{ childDeathChanceIncrease_ = clampPercnt( c ); }
		void setMinDeathChance( int c ) 			{ minDeathChance_ = clampPercnt( c ); }
		void setMaxDeathChance( int c ) 			{ maxDeathChance_ = clampPercnt( c ); }
		///@}

		/**
		* @param int To clamp
		* @return Returns the input clamped in range [0,100]
		*/
		int clampPercnt( int a ) const {
			if( a > 100 ) return 100;
			if( a <   0 ) return   0;
			return a;
		}

    private:

		struct birthInfo
		{
			int row, col;
			CellState state;
			int parentDeathChance;
            bool showChange;
			
			birthInfo(int r, int c, CellState s, int dc, bool sc)
			: row(r) ,col(c), state(s), parentDeathChance(dc), showChange(sc)
			{}
		};
		
		struct deathInfo	
		{
			int row, col;
			
			deathInfo(int r, int c)
			:row(r) ,col(c)
			{}
		};
		
		// the number of points the player has
		int high_score_;
		int currency_;

		int chanceOfMutation_;	// Liklyhood of healthy cell becoming mutated, 0.0 = 0% 1.0 = 100%
		int chanceOfCancer_;	// Liklyhood of mutated cell becoming cancerous [0.0, 1.0]

		int cancerDeathChance_;
		int adjacentBloodvesselDeathChance_;	// The death chance of cells that are adjacent to blood vessels
		int farBloodvesselDeathChance_;		// The death chance of cells that are at the limit of the blood vessel range
		int parentDeathChanceIncrease_;		// How much a parent's death chance increase each time it splits
		int childDeathChanceIncrease_;		// The death chance of childrend = dc of parent + this value
		int minDeathChance_;	// All death chances are clamped between these min and max values
		int maxDeathChance_;
		
        std::array<Hex, CHUNK_WIDTH * CHUNK_WIDTH> grid_;
            
        // A vector of hex's of type_ == NodeType::CELL
        std::vector<Hex*> cells_;

        // A vector of hex's of type_ == NodeType::BLOOD_VESSEL
        std::vector<Hex*> bloodVessels_;
        
		//list of currently active spawn points(coordinates of hex)
		std::vector<glm::vec2> bvSpawnPoints_;
		
        // Returns true if the given row/col lies on the grid
        bool hexExists( int row, int col );
		
		/**
		* When spawning a new cell, use this function to calculate the new cells death chance.
		* Takes into account the death chance of the cells parent and it's distance from BloodVessels
		* 
		* @param [in] row The row for whom the death chance will be calculated
		* @param [in] col The column for whome the death chance will be calculated
		* @param [in] parentDeathChance, The death chane of the parent of the new cell
		* @param [in] will the new cell be a cancerous cell? y:true, n:false
		* @return int An integer representing the cell's death chance.
		* A number from 0 to 100 used as a percentage
		*/
 		int calcDeathChance(int row, int col, int parentDchance, bool cancerous);
		
		// whether or not to play the blood vessel sound effect
		bool playVessel_;
    };
}

#endif
