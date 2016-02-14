#ifndef GRID_H
#define GRID_H

#include <array>
#include <vector>
#include "Hex.h"

namespace T3E
{
    class Cell;
    class BloodVessel;

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
        // [out/nullptr] the newly created cell, nullptr if the caller doesn't care
        // [ret] true if given row/col lies on board & cell created, false otherwise
        bool newCell( int row, int col, Cell** createdCell );

        // [in] row to create cell
        // [in] column to create cell
        // [out/nullptr] the newly created blood vessel, nulptr if the caller doesn't care
        // [ret] true if the givenrow/col lie on the board & bloodVessel created, false otherwise
        bool newBloodVessel( int row, int col, BloodVessel** createdBloodVessel ) {}

        // [in] row of node to be deleted
        // [in] column of node to be deleted
        void setEmpty( int row, int col );

        inline Hex* getCell( int i ) { return cells_[i]; }
        inline int numCells() { return cells_.size(); }

        inline Hex* getBloodVessel( int i ) { return bloodVessels_[i]; }
        inline int numBloodVessels() { return bloodVessels_.size(); }

    private:

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
