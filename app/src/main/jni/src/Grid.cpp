#include "Grid.h"
#include "Cell.h"
#include "BloodVessel.h"

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

    bool Grid::getNeighbours( int row, int col, Hex** neighbours )
    {
        if( !hexExists( row, col ) )
            return false;

        int i = 0, nr = row, nc = col - 1;
        
        if( !hexExists( nr, nc) ) neighbours[i] = nullptr;
        else neighbours[i] = &grid_[ nr * CHUNK_WIDTH + nc ];

        i++; nr = row + 1;

        if( !hexExists( nr, nc) ) neighbours[i] = nullptr;
        else neighbours[i] = &grid_[ nr * CHUNK_WIDTH + nc ];

        i++; nr = row - 1; nc = col;

        if( !hexExists( nr, nc) ) neighbours[i] = nullptr;
        else neighbours[i] = &grid_[ nr * CHUNK_WIDTH + nc ];

        i++; nr = row + 1;

        if( !hexExists( nr, nc) ) neighbours[i] = nullptr;
        else neighbours[i] = &grid_[ nr * CHUNK_WIDTH + nc ];

        i++; nr = row - 1; nc = col + 1;

        if( !hexExists( nr, nc) ) neighbours[i] = nullptr;
        else neighbours[i] = &grid_[ nr * CHUNK_WIDTH + nc ];

        i++; nr = row;

        if( !hexExists( nr, nc) ) neighbours[i] = nullptr;
        else neighbours[i] = &grid_[ nr * CHUNK_WIDTH + nc ];

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

    bool Grid::newBloodVessel( int row, int col, BloodVessel** createdBloodVessel )
    {
        Node* current;
        BloodVessel* newBloodVessel;

        if( getNode( row, col, &current ) )
        {
            // If there was a node already there, clean it up!
            if( current != nullptr )
                setEmpty( row, col );

            // Initialise the new blood vessel
            newBloodVessel = new BloodVessel();
                
            // Save the new blood vessel to the correct hex in the grid
            Hex* hex = &grid_[ row * CHUNK_WIDTH + col ];
            hex->setNode( (Node*)newBloodVessel );
            hex->setType( NodeType::BLOOD_VESSEL );

            // And store it in the vector
            bloodVessels_.push_back( hex );

            // Set the neighbours nodes to also point to the new blood vessel
            Hex* neighbours[6];
            for( int i = 0; i < 6; i++ )
            {
                // getNeighbours returns a nullptr to represent nodes that do not exist
                if( neighbours[i] == nullptr ) continue;

                // Delete the old contents of the node
                setEmpty( neighbours[i]->getRow(), neighbours[i]->getCol() );

                // Set the neghbours pointer to point to the newBloodVessel
                neighbours[i]->setNode( newBloodVessel );
            }

            // If the caller requested a ptr to the blood vessel, git it to them
            if( createdBloodVessel != nullptr )
                *createdBloodVessel = newBloodVessel;
        }
        else
        {
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
        // TODO: Remove blood vessels from the vector and empty their neighbours!!!!

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
