#ifndef GRID_H
#define GRID_H

#include <array>
#include <vector>
#include "Hex.h"
#include "Cell.h"
#include "BloodVessel.h"
#include "SDL.h"//log


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
		// [ret] true if a selected cell died
		// TODO: this return is quite unrelated to the function,
		// 		 but I couldn't think of a better way to unset the cellSelected_ flag in MainGame when this happens
		// call update methods of grid elements, spawn/kill cells according to sim rules
		bool update(float dTime);
		
		// [in] a hex from the grid
		// [in] another hex from the grid
		// [in] maximum range in number of hexes 
		// [ret] true if the distance between the two hexes is not greater than the range given
		bool inRange(int rowA, int colA ,int rowB, int colB, int range);
		
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
		bool spawnCell(int selRow, int selCol, int touchRow, int touchCol);
		
		//TODO: does this make getNode redundant?
		// [in] row
        // [in] column
        // [out] hex at given row column if found
        // [ret] true if given row/col lies on board, false otherwise
		bool getHex(int row, int col, Hex** hex);
		
		//[ret] size of one of the grid's dimensions
		//		assuming height = width
		int getSize(){return CHUNK_WIDTH;};
		
		//TODO:kinda meh designed function, but i'm tired, pls rethink this
		// [in] row of hex to be queried
		// [in] column of hex to be queried
		// [ret] vec3 where x and y are world coords of hex
		//		z= 1 if hex is in blood vessel range or 0 otherwise
		//		x and y will be -1 if hex doesnt exist(our grid is all in positive coords so that works for now)
		glm::vec3 getHexDrawInfo(int row, int col);
		
		// [in] row of currently selected cell
		// [in] column of currently selected cell
		// [in] row of touched hex
		// [in] column of touched hex
		// [ret] true if stem was moved false otherwise
		bool moveStemCell(int selRow, int selCol, int touchRow, int touchCol);
		
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
		
        std::array<Hex, CHUNK_WIDTH * CHUNK_WIDTH> grid_;
            
        // A vector of hex's of type_ == NodeType::CELL
        std::vector<Hex*> cells_;

        // A vector of hex's of type_ == NodeType::BLOOD_VESSEL
        std::vector<Hex*> bloodVessels_;
        
        // Returns true if the given row/col lies on the grid
        bool hexExists( int row, int col );
    };
}

#endif
