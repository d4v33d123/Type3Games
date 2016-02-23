#include "Grid.h"


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
		//free memory
		for (std::vector<Hex*>::iterator it = cells_.begin() ; it != cells_.end(); ++it)
		{
			delete (*it);
		} 
		cells_.clear();
		for (std::vector<Hex*>::iterator it = bloodVessels_.begin() ; it != bloodVessels_.end(); ++it)
		{
			delete (*it);
		} 
		bloodVessels_.clear();
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

    bool Grid::newCell( int row, int col, CellState state, int deathChance, Cell** createdCell )
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
			newCell->init(state, deathChance);						
            
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
            const int hexPos = row * CHUNK_WIDTH + col;
            grid_[ hexPos ].setNode( (Node*)newBloodVessel );
            grid_[ hexPos ].setType( NodeType::BLOOD_VESSEL_CORE );

            // And store it in the vector
            bloodVessels_.push_back( &grid_[ hexPos ] );

            // Set the neighbours nodes to also point to the new blood vessel
            Hex* neighbours[6];
            getNeighbours( row, col, neighbours );
            for( int i = 0; i < 6; i++ )
            {
                // getNeighbours returns a nullptr to represent nodes that do not exist
                if( neighbours[i] == nullptr ) continue;

                // Delete the old contents of the node
                setEmpty( neighbours[i]->getRow(), neighbours[i]->getCol() );

                // Set the neghbours pointer to point to the newBloodVessel
                neighbours[i]->setNode( newBloodVessel );
                neighbours[i]->setType( NodeType::BLOOD_VESSEL_EDGE );
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
        else if( grid_[ hexPos ].getType() == NodeType::BLOOD_VESSEL_EDGE )
        {
            // If the node is the edge of a blood vessel
            // Search for the core of the blood vessel
            Hex* neighbours[6];
            getNeighbours( row, col, neighbours );
            for( int i = 0; i < 6; i++ )
            {
                if( neighbours[i]->getType() == NodeType::BLOOD_VESSEL_CORE )
                {
                    //SDL_Log("Found edge");
                    // then call setEmpty on the core!
                    setEmpty( neighbours[i]->getRow(), neighbours[i]->getCol() );
                    return;
                }
            }

        }
        else if( grid_[ hexPos ].getType() == NodeType::BLOOD_VESSEL_CORE )
        {
            // Remove the blood vessel from the vector
            for( auto it = bloodVessels_.begin(); it != bloodVessels_.end(); ++it )
            {
                if( *it == &grid_[ hexPos ] )
                {
                    bloodVessels_.erase( it );
                    break;
                }
            }

            // Set the edges to empty
            Hex* neighbours[6];
            getNeighbours( row, col, neighbours );
            for( int i = 0; i < 6; i++ )
            {
                grid_[ neighbours[i]->getRow() * CHUNK_WIDTH + neighbours[i]->getCol() ].setNode( nullptr );
                grid_[ neighbours[i]->getRow() * CHUNK_WIDTH + neighbours[i]->getCol() ].setType( NodeType::EMPTY );
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
        if( row >= CHUNK_WIDTH ) return false;
        if( col >= CHUNK_WIDTH ) return false;

        return true;
    }
	
	bool Grid::inRange(int rowA, int colA ,int rowB, int colB, int range)
	{		
		//calculate the squared distance (avoid sqrt operation)
		int dSquared = (std::abs(colA - colB) + std::abs(rowA - rowB) + std::abs((-colA-rowA) - (-colB-rowB))) / 2;
		if(dSquared > range)
			return false;
		return true;
	}
	
	bool Grid::update(float dTime)
	{		
		bool selectedCellDied = false;
		
		//TODO: use queue/list?
		std::vector<birthInfo> newCells;
		std::vector<deathInfo> deadCells;
	
		//update cells
		for(std::vector<Hex*>::iterator hex = cells_.begin(); hex != cells_.end(); ++hex)
        {
			//get the hex's node and cast it to cell
			Cell* current = (Cell*)((*hex)->getNode());
			//update cell, check if it's time to split
			if(current->update(dTime))
			{
				//roll for chance to die
				int die = rand()%100;
				if(die >= current->getDeathChance())
				{
					//get the neighbouring hexes
					Hex* neighbours[6];
					getNeighbours((*hex)->getRow(), (*hex)->getCol(), neighbours);
					//pick a lucky neighbor and check if it's empty
					int lucky = rand()%6;
					if(neighbours[lucky] != nullptr)
					{
						if(neighbours[lucky]->getType() == NodeType::EMPTY)
						{					
							//rolls
							int noMutation;
							int noCancer;
							
							//is in blood vessel range?
							bool inBvRange = false;
							bool inBvCancerRange = false;

							//check if the new cell is in the range of a blood vessel
							for(std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs)
							{
								BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
								//range 2 from centre of bv, so adjacent
								if(inRange((*bvs)->getRow(), (*bvs)->getCol(), neighbours[lucky]->getRow(), neighbours[lucky]->getCol(), bloodVessel->getRange()))
								{
									inBvRange = true;
								}
								if(inRange((*bvs)->getRow(), (*bvs)->getCol(), neighbours[lucky]->getRow(), neighbours[lucky]->getCol(), bloodVessel->getRange()*2))
									inBvCancerRange = true;
									
							}
							
							//create a new cell depending on current's type
							switch(current->getState())
							{
							case CellState::STEM:
								if(inBvRange)
								{
									//spawn normal cell with 5% death chance
									newCells.push_back(birthInfo(neighbours[lucky]->getRow(), neighbours[lucky]->getCol(), CellState::NORMAL, 5));
								}
								break;
								
							case CellState::NORMAL:
								if(inBvRange)
								{
									//increase parent death chance by 5%
									current->incDeathChance(5);
									//roll for mutation
									noMutation = rand()%100;
									if(noMutation)
									{
										//spawn normal cell with parent's death chance
										newCells.push_back(birthInfo(neighbours[lucky]->getRow(), neighbours[lucky]->getCol(), CellState::NORMAL, current->getDeathChance()));
									}
									else
									{
										//spawn mutated cell with parent's death chance
										newCells.push_back(birthInfo(neighbours[lucky]->getRow(), neighbours[lucky]->getCol(), CellState::MUTATED, current->getDeathChance()));
									}
									break;
								}
								
							case CellState::MUTATED:								
								//roll for cancer
								noCancer = rand()%100;
								if(noCancer)
								{
									if(inBvRange)
									{
										//increase parent death chance by 5%
										current->incDeathChance(5);
										//spawn mutated cell with parent's death chance
										newCells.push_back(birthInfo(neighbours[lucky]->getRow(), neighbours[lucky]->getCol(), CellState::MUTATED, current->getDeathChance()));
									}
									
								}
								else
								{
									//if(inBvCancerRange)
									//{	
									//increase parent death chance by 5%
									current->incDeathChance(5);
									//spawn cancerous cell
									newCells.push_back(birthInfo(neighbours[lucky]->getRow(), neighbours[lucky]->getCol(), CellState::CANCEROUS, 0));
									//}
								}
								break;
								
							case CellState::CANCEROUS:
								if(inBvCancerRange)
								{	
									//spawn cancerous cell
									newCells.push_back(birthInfo(neighbours[lucky]->getRow(), neighbours[lucky]->getCol(), CellState::CANCEROUS, 0));
								}
								break;
								
							default:
								break;
							}
						}
					}
	
				}
				//if the cell died
				else
				{
					selectedCellDied = current->isSelected();
					deadCells.push_back(deathInfo((*hex)->getRow(), (*hex)->getCol()));
				}
			}		
		}
		//remove dead cells
		for(std::vector<deathInfo>::iterator c = deadCells.begin(); c != deadCells.end(); ++c)
		{
			setEmpty( c->row, c->col );
		}
		//add new cells
		for(std::vector<birthInfo>::iterator c = newCells.begin(); c != newCells.end(); ++c)
		{
			newCell( c->row, c->col, c->state, c->deathChance, nullptr );
		}
		
		return selectedCellDied;
	}
	
	bool Grid::arrestCell(int row, int col)
	{
		// If the hex does not lie on the board or is not a cell, return error
        if( (!hexExists( row, col )) || (grid_[row * CHUNK_WIDTH + col].getType() != NodeType::CELL))
		{
			return false;
		}
            
		Cell* cell = (Cell*)(grid_[row * CHUNK_WIDTH + col].getNode());
		
		//only arrest normal cells
		if(cell->getState() == CellState::NORMAL)
		{
			cell->arrest();
			return true;
		}
		return false;			
	}
	
	bool Grid::selectCell(int row, int col)
	{
		 // If the hex does not lie on the board or is not a cell, return error
        if( (!hexExists( row, col )) || (grid_[row * CHUNK_WIDTH + col].getType() != NodeType::CELL))
            return false;
		
		Cell* cell = (Cell*)(grid_[row * CHUNK_WIDTH + col].getNode());
		//only select normal and stem cells
		if((cell->getState() == CellState::NORMAL) || (cell->getState() == CellState::STEM))
		{
			cell->select();
			return true;
		}
		
		return false;			
	}
	
	bool Grid::unselectCell(int row, int col)
	{
		// If the hex does not lie on the board or is not a cell, return error
        if( (!hexExists( row, col )) || (grid_[row * CHUNK_WIDTH + col].getType() != NodeType::CELL))
            return false;
		
		Cell* cell = (Cell*)(grid_[row * CHUNK_WIDTH + col].getNode());
		//only select normal and stem cells
		if((cell->getState() == CellState::NORMAL) || (cell->getState() == CellState::STEM))
		{
			cell->unselect();
			return true;
		}
		
		return false;			
	}
	
	bool Grid::spawnCell(int selRow, int selCol, int touchRow, int touchCol)
	{
		// If the hex does not lie on the board or is not a cell, return error
        if( (!hexExists( selRow, selCol )) || (grid_[selRow * CHUNK_WIDTH + selCol].getType() != NodeType::CELL))
            return false;
		
		Cell* selectedCell = (Cell*)(grid_[selRow * CHUNK_WIDTH + selCol].getNode());
		
		//a stem cell must be in alternate mode to spawn
		if(selectedCell->getState() == CellState::STEM)
			if(!selectedCell->isInAlternateMode())
				return false;

		// get the neighbours of the currently selected cell
		T3E::Hex* neighbours[6];
		getNeighbours( selRow, selCol, neighbours );
		//check if the touched position is one of neighbours of the selected cell
		for(int i = 0; i < 6; ++i)
		{
			if(neighbours[i] != nullptr)
			{
				if((neighbours[i]->getRow() == touchRow) && (neighbours[i]->getCol() == touchCol))
				{
					//if it's empty
					if(isEmpty(touchRow, touchCol))
					{
						//check if is in the range of at least one blood vessel
						for( std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs )
						{
							BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
							if(inRange((*bvs)->getRow(), (*bvs)->getCol(), touchRow, touchCol, bloodVessel->getRange()))
							{
								//spawn new cell with parent's death chance + 5
								if(newCell(touchRow, touchCol, selectedCell->getState(), selectedCell->getDeathChance() + 5, nullptr))
								{
									//now we increase parent's death chance aswell if it was normal
									if(selectedCell->getState() == CellState::NORMAL)
										selectedCell->incDeathChance(5);
									return true;
								}									
							}
						}
					}
					else
						break;
				}
			}
		}
		
		//couldn't create cell
		return false;
	}
	
	bool Grid::moveStemCell(int selRow, int selCol, int touchRow, int touchCol)
	{
		// If the hex does not lie on the board or is not a cell, return error
        if( (!hexExists( selRow, selCol )) || (grid_[selRow * CHUNK_WIDTH + selCol].getType() != NodeType::CELL))
            return false;

		Cell* selectedCell = (Cell*)(grid_[selRow * CHUNK_WIDTH + selCol].getNode());
		
		//a not a stem cell selected, return error
		if(selectedCell->getState() != CellState::STEM)
			return false;
			
		// get the neighbours of the currently selected cell
		T3E::Hex* neighbours[6];
		getNeighbours( selRow, selCol, neighbours );
		//check if the touched position is one of neighbours of the selected cell
		for(int i = 0; i < 6; ++i)
		{
			if(neighbours[i] != nullptr)
			{
				if((neighbours[i]->getRow() == touchRow) && (neighbours[i]->getCol() == touchCol))
				{
					//if it's empty, spawn a cell
					if(isEmpty(touchRow, touchCol))
					{
						//check if is in the range of at least one blood vessel
						for( std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs )
						{
							BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
							if(inRange((*bvs)->getRow(), (*bvs)->getCol(), touchRow, touchCol, bloodVessel->getRange()))
							{
								if(newCell(touchRow, touchCol, CellState::STEM, 0, nullptr))
								{
									//delete the old stem cell
									setEmpty(selRow, selCol);
									return true;
								}							
								//couldn't create cell
								return false;
							}
						}
					}
					else
						break;
				}
			}
		}
		//not one of the neighbours
		return false;
	}
	
	bool Grid::getHex(int row, int col, Hex** hex)
	{
		 // If the hex does not lie on the board, return error
        if( !hexExists( row, col ) )
            return false;

        // get that hex
        *hex = &(grid_[ row * CHUNK_WIDTH + col ]);

        return true;
	}
	
	glm::vec3 Grid::getHexDrawInfo(int row, int col)
	{
		glm::vec3 data;
        if( hexExists( row, col ) )
			data = glm::vec3(grid_[ row * CHUNK_WIDTH + col ].getX(), grid_[ row * CHUNK_WIDTH + col ].getY(), 0);
		
		//check if in range of a blood vessel
		for( std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs )
		{
			BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
			//range 2 from centre of bv, so adjacent
			if(inRange((*bvs)->getRow(), (*bvs)->getCol(), row, col, bloodVessel->getRange()))
			{
				data.z = 1;//will be drawn red
			}
		}
		
		return data;
	}
	
	bool Grid::setStemToSpawnMode(int row, int col)
	{
		// If the hex does not lie on the board or is not a cell, return error
        if( (!hexExists( row, col )) || (grid_[row * CHUNK_WIDTH + col].getType() != NodeType::CELL))
            return false;
		
		Cell* cell = (Cell*)(grid_[row * CHUNK_WIDTH + col].getNode());
		//only arrest normal cells
		if(cell->getState() == CellState::STEM)
		{
			cell->toggleMode();
			return true;
		}
		
		return false;		
	}
	
	bool Grid::growBloodVesselAt( int row, int col )
	{
		T3E::Hex* neighbours[6];
		int adjacentCells = 0;

		if(!hexExists( row, col ))
			return false;

		// return immediately if the growth coord is not empty
		if( !isEmpty( row, col ) )
			return false;

		// Check the cell has a live neighbour
		if( getNeighbours( row, col, neighbours ) )
		{
			// Count the number of adjacent cells
			for( int i = 0; i < 6; i++ )
			{
				if( neighbours[i] != nullptr )
				{
					if(neighbours[i]->getType() == T3E::NodeType::CELL)
					{
						adjacentCells++;
					}
						
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
					
			}
		}

		if( adjacentCells == 6 )
		{
			newBloodVessel( row, col, nullptr );
			return true;
		}
	}
}