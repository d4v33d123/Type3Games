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

        // [in] row
        // [in] column
        // [out] node at given row column if found
        // [ret] true if given row/col lies on board, false otherwise
        bool getNode( int row, int col, Node** node );
		
        // [in] row of central node
        // [in] column of central node
        // [out] pointer to Hex*[6], array must have 6 elements!
        // Nodes that are uninitalised have NodeType::EMPTY
        // Nodes that do not exist are returned as nullptrs
        bool getNeighbours( int row, int col, Hex** neighbours );

        // [in] row to create cell
        // [in] column to create cell
		// [in] cell state (see CellState enum in Cell.h)
		// [in] death chance; this is ignored if the new cell is stem or cancerous
        // [out/nullptr] the newly created cell, nullptr if the caller doesn't care
        // [ret] true if given row/col lies on board & cell created, false otherwise
        bool newCell( int row, int col, CellState state, int deathChance, Cell** createdCell);

        // [in] row to create cell
        // [in] column to create cell
        // [out/nullptr] the newly created blood vessel, nulptr if the caller doesn't care
        // [ret] true if the givenrow/col lie on the board & bloodVessel created, false otherwise
        bool newBloodVessel( int row, int col, BloodVessel** createdBloodVessel );

        // [in] row of node to be deleted
        // [in] column of node to be deleted
        void setEmpty( int row, int col );

        // TODO: decide if this shoud return true of false if the hex doesn't exist, which is more intuitive?
        // [ret] false if the hex doesn't exist, true if it exists and is empty, false if it exists and is not empty
        inline bool isEmpty( int row, int col ) { if( !hexExists( row, col ) ) return false; return grid_[row*CHUNK_WIDTH+col].getType() == NodeType::EMPTY; }

        inline Hex* getCell( int i ) { return cells_[i]; }
        inline int numCells() { return cells_.size(); }

        inline Hex* getBloodVessel( int i ) { return bloodVessels_[i]; }
        inline int numBloodVessels() { return bloodVessels_.size(); }

		// [in] time since last frame was rendered
		// [in] finger position (row column), to avoid killing cell that is being hovered
		// [ret] true if a selected cell died
		// TODO: this return is quite unrelated to the function,
		// 		 but I couldn't think of a better way to unset the cellSelected_ flag in MainGame when this happens
		// call update methods of grid elements, spawn/kill cells according to sim rules
		bool update(float dTime, SDL_Point fingerRowCol);
		
		//TODO: use hexexist checks in getdistance and inrange ? or not since we check before anyway since it's only used internally?
		
		// [in] a hex from the grid
		// [in] another hex from the grid
		// [in] maximum range in number of hexes 
		// [ret] true if the distance between the two hexes is not greater than the range given
		bool inRange(int rowA, int colA ,int rowB, int colB, int range);
		
		// [in] row of a hex from the grid
		// [in] col of a hex from the grid
		// [in] row of another hex from the grid
		// [in] col of another hex from the grid
		// [ret] distance in n of hexes between the two given hex coords
		int getDistance(int rowA, int colA ,int rowB, int colB);
		
		// [in] row of cell to arrest
		// [in] column of cell to arrest
		// [ret] false if cell doesn't exist or is not CellState::NORMAL, true if success
		bool arrestCell(int row, int col);
		
		// [in] row of cell to select
		// [in] column of cell to select
		// [ret] false if cell doesn't exist or is not CellState::NORMAL/STEM, true if success
		bool selectCell(int row, int col);
		
		// [in] row of cell to unselect
		// [in] column of cell to unselect
		// [ret] false if cell doesn't exist or is not CellState::NORMAL/STEM, true if success
		bool unselectCell(int row, int col);
		
		// [in] row of currently selected cell
		// [in] column of currently selected cell
		// [in] row of touched hex
		// [in] column of touched hex
		// [ret] true if new cell is created, false otherwise
		// calles when player tries to spawn cell manually
		bool spawnCell(int selRow, int selCol, int touchRow, int touchCol);
		
		// [in] row of cell to delete
		// [in] column of cell to delete
		// [ret] true if new cell is deleted, false otherwise
		// calles when player tries to kill cell manually
		bool killCell(int row, int col);
		
		//TODO: does this make getNode redundant?
		// [in] row
        // [in] column
        // [out] hex at given row column if found
        // [ret] true if given row/col lies on board, false otherwise
		bool getHex(int row, int col, Hex** hex);
		
		//[ret] size of one of the grid's dimensions
		//		assuming height = width
		int getSize(){return CHUNK_WIDTH;};
		
		//TODO:kinda meh designed function?...
		// [in] row of hex to be queried
		// [in] column of hex to be queried
		// [ret] vec3 where x and y are world coords of hex
		//		z = 0 if hex is in blood vessel range ,1 if in large range, 2 otherwise
		//		w = lerp factor for colour
		//		x and y will be -1 if hex doesnt exist(our grid is all in positive coords so that works for now)
		glm::vec4 getHexDrawInfo(int row, int col, bool cellSelected, glm::vec2 selectedPos, InteractionMode interactionMode_);
		
		// [in] row of currently selected cell
		// [in] column of currently selected cell
		// [in] row of touched hex
		// [in] column of touched hex
		// [ret] true if stem was moved false otherwise
		bool moveStemCell(int selRow, int selCol, int touchRow, int touchCol);
		
		// [in] row of cell to test
		// [in] column of cell to test
		// [ret] false if cell doesn't exist or is not CellState::STEM, true if success
		// sets stem cell in spawn mode
		bool setStemToSpawnMode(int row, int col);
		
		// [in] row to test
		// [in] column to test
		// [ret] true if a bv was created, false otherwise
		// try to create a blood vessel at the specified position
		bool growBloodVesselAt( int row, int col );
		
		// [ret] true if to play blood vessel sound
		inline bool playVessel() { return playVessel_; }
		
		// resets the blood vessel sound effect 
		inline void resetPlayVessel() { playVessel_ = false; }
				
		// [ret] the score
		inline int getScore() { return score_; }

		// [in] row to test
		// [in] column to test
		// [ret] true if success
		// try to create a blood vessel spawn point at the specified position
		bool setBvSpawn(int row, int col);
		
		// [in] bv spawn point that we want to know the coordinates of
		glm::vec2 getBvSpawnCoords( int i );
		
		int numBvSpawns() { return bvSpawnPoints_.size(); }

    private:

		struct birthInfo
		{
			int row, col;
			CellState state;
			int deathChance;
			
			birthInfo(int r, int c, CellState s, int dc)
			:row(r) ,col(c), state(s), deathChance(dc)
			{}
		};
		
		struct deathInfo	
		{
			int row, col;
			
			deathInfo(int r, int c)
			:row(r) ,col(c)
			{}
		};
		
		// the score
		int score_;
		
        std::array<Hex, CHUNK_WIDTH * CHUNK_WIDTH> grid_;
            
        // A vector of hex's of type_ == NodeType::CELL
        std::vector<Hex*> cells_;

        // A vector of hex's of type_ == NodeType::BLOOD_VESSEL
        std::vector<Hex*> bloodVessels_;
        
		//list of currently active spawn points(coordinates of hex)
		std::vector<glm::vec2> bvSpawnPoints_;
		
        // Returns true if the given row/col lies on the grid
        bool hexExists( int row, int col );
		
		//[in] row new cell will spawn in
		//[in] col new cell will spawn in
		//[in] parent's death chance
		//[in] will the new cell be a cancerous cell? y:true, n:false
		//[ret] new cell's death chance
 		int calcDeathChance(int row, int col, int parentDchance, bool cancerous);
		
		// whether or not to play the blood vessel sound effect
		bool playVessel_;
    };
}

#endif
