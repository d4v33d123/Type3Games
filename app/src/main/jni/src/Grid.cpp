#include "Grid.h"
#include "Cell.h"

namespace T3E
{
    Grid::Grid()
    {
        // Initialise all the cells ( to empty )
        for( int col = 0; col < CHUNK_WIDTH; col++ )
        for( int row = 0; row < CHUNK_WIDTH; row++ )
                grid_[ row * CHUNK_WIDTH + col ].init( row, col );
    }

    Grid::~Grid()
    {
    }

    bool Grid::getNode( int row, int col, Node** node )
    {
        // If the hex does not lie on the board, return error
        if( !hexExists( row, col ) )
            return false;

        // get that hex's node
        *node = grid_[ row * CHUNK_WIDTH + col ].getNode();

        return true;
    }

    bool Grid::newCell( int row, int col, Cell** createdCell )
    {
        Node* current;
        Cell* newCell;

        if( getNode( row, col, &current ) )
        {
            // If there was a node already there, clean it up
            if( current != nullptr )
                setEmpty( row, col );

            // Intialise the new cell
            newCell = new Cell();

            // Save the new cell to the correct hex in the grid
            Hex* hex = &grid_[ row * CHUNK_WIDTH + col ];
            hex->setNode( (Node*)newCell );
            hex->setType( NodeType::CELL );

            // And store it in the vector
            cells_.push_back( hex );

            // If the caller requested the new cell, pass it to them
            if( createdCell != nullptr )
                *createdCell = newCell;
        }
        else
        {
            // The requested coords do not lie on the grid
            return false;
        }

        return true;
    }

    void Grid::setEmpty( int row, int col )
    {
        if( !hexExists( row, col ) )
            return;

        int hexPos = row * CHUNK_WIDTH + col;
        Node* nodeToDelete = grid_[ hexPos ].getNode();

        // If the hex's node it tracked using on of the vectors, it needs to be removed from said vector
        if( grid_[ hexPos ].getType() == NodeType::CELL )
        {
            for( auto it = cells_.begin(); it != cells_.end(); ++it )
            {
                if( *it == &grid_[ hexPos ] )
                {
                    cells_.erase( it );
                    break;
                }
            }
        }

        // Delete the node itself, set the hex's node to nullptr and set the hex to emtpy, 
        delete nodeToDelete;
        grid_[ hexPos ].setNode( nullptr );
        grid_[ hexPos ].setType( NodeType::EMPTY );
    }

    bool Grid::hexExists( int row, int col )
    {
        if( row < 0 ) return false;
        if( col < 0 ) return false;
        if( row > CHUNK_WIDTH - 1 ) return false;
        if( col > CHUNK_WIDTH - 1 ) return false;

        return true;
    }
}
