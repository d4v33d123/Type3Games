#include "Grid.h"
#include "GlobalScoreValues.h"

namespace T3E
{
    Grid::Grid() :
	playVessel_(false)
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

    bool Grid::newCell( int row, int col, CellState state, int parentDeathChance, Cell** createdCell )
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
			newCell->init( state, calcDeathChance( row, col, parentDeathChance, state == T3E::CellState::CANCEROUS ) );
            
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

        // Give the player points based on what has spawned
        switch( state )
        {
        	case CellState::STEM:
        		score_ += T3E::SCORE::SPAWNED_STEM_CELL();
        	break;
        	case CellState::NORMAL:
        		score_ += T3E::SCORE::SPAWNED_HEALTHY_CELL();
        	break;
        	case CellState::MUTATED:
        		score_ += T3E::SCORE::SPAWNED_MUTATED_CELL();
        	break;
        	case CellState::CANCEROUS:
        		score_ += T3E::SCORE::SPAWNED_CANCER_CELL();
        	break;
        	default:
        		// No points for you
        	break;
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

        score_ += T3E::SCORE::SPAWNED_BLOODVESSEL();

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

            // Update the players score based on the type of cell removed
	        switch( ((Cell*)nodeToDelete)->getState() )
	        {
	        	case CellState::STEM:
	        		//score_ += T3E::SCORE::KILLED_STEM_CELL();
	        	break;
	        	case CellState::NORMAL:
	        		score_ += T3E::SCORE::KILLED_HEALTHY_CELL();
	        	break;
	        	case CellState::MUTATED:
	        		score_ += T3E::SCORE::KILLED_MUTATED_CELL();
	        	break;
	        	case CellState::CANCEROUS:
	        		score_ += T3E::SCORE::KILLED_CANCER_CELL();
	        	break;
	        	case CellState::ARRESTED:
	        		score_ += T3E::SCORE::KILLED_ARRESTED_CELL();
	        	break;
	        	default:
	        		// No points for you
	        	break;
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

            score_ += T3E::SCORE::KILLED_BLOODVESSEL();
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
	
	int Grid::calcDeathChance(int row, int col, int parentDchance, bool cancerous)
	{
		// check distance to closest blood vessel
		float inRange = false;
		int distToBv = 999; // high number to make sure first if is true
		int death_chance; // death chance
		int bloodvessel_range = T3E::BloodVessel::getRange();

		for(std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs)
		{
			BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
			int dist = getDistance((*bvs)->getRow(), (*bvs)->getCol(), row, col);
			if(dist < distToBv)
				distToBv = dist;		
		}			
		
		if( cancerous )
		{
			// All cancer cells are given a fixed death chance because they don't give a fuck
			// TODO: this should probably changed to some kind of formula, ask the designers...
			death_chance = cancerDeathChance_;
		}
		else
		{
			if( distToBv <= 2 )
			{
				// The hex is adjacent to the blood vessel
				death_chance = adjacentBloodvesselDeathChance_; 
				//SDL_Log("Adjacent cell distance %i dc %i", distToBv, death_chance);
			}
			else if( distToBv <= bloodvessel_range )
			{
				// take account of adjacent cells are two away from the blood vessel
				float lerpAmount = (distToBv - 2) / float(bloodvessel_range - 2);

				// As the cell gets farther away from the blood vessel
				// Lerp from the adjacent_death_chance to the far_death_chance
				death_chance = (adjacentBloodvesselDeathChance_ * (1.0f - lerpAmount)) + (farBloodvesselDeathChance_ * lerpAmount);
				//SDL_Log("adjbvdc %i farbvdc %i", adjacentBloodvesselDeathChance_, farBloodvesselDeathChance_ );
				//SDL_Log("In range cell lerp %f, dc %i", lerpAmount, death_chance);
			}
			else
			{
				// If the cell is outside the range of a blood vessel
				// It's death chance is based on it's parents + some value
				death_chance = parentDchance + childDeathChanceIncrease_;
				//SDL_Log("Distant cell, %i parentDC %i dc %i", distToBv, parentDchance, death_chance);
			}			
		}
		
		//cap death_chance
		if(death_chance < minDeathChance_ ) death_chance = minDeathChance_;
		if(death_chance > maxDeathChance_ ) death_chance = maxDeathChance_;
		
		return death_chance;
	}
	
	bool Grid::update(float dTime, SDL_Point fingerRowCol)
	{
		bool selectedCellDied = false;
		
		//TODO: use queue/list?
		std::vector<birthInfo> newCells;
		std::vector<deathInfo> deadCells;
	
		//try to create blood vessels at spawn points
		for(std::vector<glm::vec2>::iterator sp = bvSpawnPoints_.begin(); sp != bvSpawnPoints_.end();)
		{
			if(growBloodVesselAt(sp->x, sp->y))
			{
				sp = bvSpawnPoints_.erase(sp);
				playVessel_ = true;
			}
			else
				++sp;
		}	
		
		//update bvs
		for( std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs )
		{
			BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
			bloodVessel->update(dTime);
		}
		
		//update cells
		for(std::vector<Hex*>::iterator hex = cells_.begin(); hex != cells_.end(); ++hex)
        {
			//get the hex's node and cast it to cell
			Cell* current = (Cell*)((*hex)->getNode());

			if(current->isDying())
			{
				if(current->isDead())
					deadCells.push_back(deathInfo((*hex)->getRow(), (*hex)->getCol()));
				else
					current->die(dTime);//do this after isDead() check so we don't skip rendering the last frame
			}
			//update cell, check if it's time to split
			else if(current->update(dTime))
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
							int random_val = rand() & 255;
							
							// create a new cell depending on current's type
							switch(current->getState())
							{
							case CellState::STEM:
								newCells.push_back( birthInfo(
									neighbours[lucky]->getRow(),
									neighbours[lucky]->getCol(),
									CellState::STEM,
									current->getDeathChance() ));

								break;								
							case CellState::NORMAL:
								if( random_val < (chanceOfMutation_ / 100.0f) * 255 )
								{
									newCells.push_back( birthInfo(
										neighbours[lucky]->getRow(),
										neighbours[lucky]->getCol(),
										CellState::MUTATED,
										current->getDeathChance() ));
								}
								else
								{
									newCells.push_back( birthInfo(
										neighbours[lucky]->getRow(),
										neighbours[lucky]->getCol(),
										CellState::NORMAL,
										current->getDeathChance() ));
								}
								break;								
							case CellState::MUTATED:
								if( random_val < (chanceOfCancer_ / 100.0f) * 255 )
								{
									newCells.push_back( birthInfo(
										neighbours[lucky]->getRow(),
										neighbours[lucky]->getCol(),
										CellState::CANCEROUS,
										current->getDeathChance() ));
								}
								else
								{
									newCells.push_back( birthInfo(
										neighbours[lucky]->getRow(),
										neighbours[lucky]->getCol(),
										CellState::MUTATED,
										current->getDeathChance() ));
								}
								break;								
							case CellState::CANCEROUS: // Cancerous cells always spawn more cancerous cells
								newCells.push_back( birthInfo(
									neighbours[lucky]->getRow(),
									neighbours[lucky]->getCol(),
									CellState::CANCEROUS,
									current->getDeathChance() ));
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
					//make sure the player is not hovering the finger on this cell
					if(fingerRowCol.x != (*hex)->getRow() || fingerRowCol.y != (*hex)->getCol())
					{
						selectedCellDied = current->isSelected();
						if(selectedCellDied)
							current->unselect();
						current->kill();
					}
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
			// If it's parent was a stem cell, it gets the minimum death chance
			if( c->state == CellState::STEM )
			{
				Cell* stemChild;
				newCell( c->row, c->col, CellState::NORMAL, 0, &stemChild );
				stemChild->setDeathChance( minDeathChance_ );
			}
			else
			{
				newCell( c->row, c->col, c->state, c->parentDeathChance, nullptr );
			}
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
		if(cell->getState() == CellState::NORMAL && score_ + T3E::SCORE::ARRESTED_CELL() > 0 )
		{
			cell->arrest();
			score_ += T3E::SCORE::ARRESTED_CELL();
			return true;
		}
		else
		{
			return false;
		}
	}
	
	bool Grid::selectCell(int row, int col)
	{
		 // If the hex does not lie on the board or is not a cell, return error
        if( (!hexExists( row, col )) || (grid_[row * CHUNK_WIDTH + col].getType() != NodeType::CELL)) {
            return false;
        }
		
		Cell* cell = (Cell*)(grid_[row * CHUNK_WIDTH + col].getNode());
		
		if(cell->isDying() || cell->isDead())
			return false;
		
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

		// TODO: unravel this crazy nested logic, i see bugs in here and unexpected looping given certain conditions
		//check if the touched position is one of neighbours of the selected cell
		for(int i = 0; i < 6; ++i)
		{
			if(neighbours[i] != nullptr)
			{
				if((neighbours[i]->getRow() == touchRow) && (neighbours[i]->getCol() == touchCol))
				{
					if(isEmpty(touchRow, touchCol))
					{
						// spawn new cell, pass the selected cells de as it's parent
						if(newCell(touchRow, touchCol, selectedCell->getState(), selectedCell->getDeathChance(), nullptr))
						{
							//now we increase parent's death chance aswell if it was normal
							if(selectedCell->getState() == CellState::NORMAL)
								// TODO: why is incDeathChance even a method? shoud be set( get + inc ) --- because incdeathchance also checks the increase doesn't add up to over 99
								selectedCell->incDeathChance( parentDeathChanceIncrease_ );

							return true;
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
	
	bool Grid::killCell(int row, int col)
	{
		 // If the hex does not lie on the board or is not a cell, return error
        if( (!hexExists( row, col )) || (grid_[row * CHUNK_WIDTH + col].getType() != NodeType::CELL))
            return false;
		
		Cell* cell = (Cell*)(grid_[row * CHUNK_WIDTH + col].getNode());
		//different costs for normal and mutated cells
		if( cell->getState() == CellState::NORMAL && score_ - T3E::SCORE::KILLED_HEALTHY_CELL() > 0 )
		{
			cell->kill();
			return true;
		}
		else if( cell->getState() == CellState::MUTATED && score_ - T3E::SCORE::KILLED_MUTATED_CELL() > 0 )
		{
			cell->kill();
			return true;
		}
		else if( cell->getState() == CellState::ARRESTED && score_ - T3E::SCORE::KILLED_ARRESTED_CELL() > 0 )
		{
			cell->kill();
			return true;
		}
		else
		{
			return false;
		}
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
					Cell* cell = (Cell*)(grid_[touchRow * CHUNK_WIDTH + touchCol].getNode());
					
					//if it's empty, spawn a cell
					if(isEmpty(touchRow, touchCol))
					{
						//check if is in the range of at least one blood vessel
						for( std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs )
						{
							BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
							if(inRange((*bvs)->getRow(), (*bvs)->getCol(), touchRow, touchCol, bloodVessel->getRange()*1.5f))
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
					else if(cell->getState() == CellState::NORMAL)
					{
						int dc = cell->getDeathChance();
						glm::vec4 tint = cell->getTint();
						//check if is in the range of at least one blood vessel
						for( std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs )
						{
							BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
							if(inRange((*bvs)->getRow(), (*bvs)->getCol(), touchRow, touchCol, bloodVessel->getRange()*1.5f))
							{
								setEmpty(touchRow, touchCol);
								if(newCell(touchRow, touchCol, CellState::STEM, 0, nullptr))
								{
									//move the normal cell to the stem cell's new pos
									newCell( selRow, selCol, CellState::NORMAL, dc, nullptr);
									Cell* nCell = (Cell*)(grid_[selRow * CHUNK_WIDTH + selCol].getNode());
									nCell->setTint(tint);
									
									return true;
								}				
								// couldn't move the stem cell so put the normal cell back 
								newCell( touchRow, touchCol, CellState::NORMAL, dc, nullptr);
								Cell* nCell = (Cell*)(grid_[touchRow * CHUNK_WIDTH + touchCol].getNode());
								nCell->setTint(tint);
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
	
	glm::vec4 Grid::getHexDrawInfo(int row, int col, bool cellSelected, glm::vec2 selectedPos, InteractionMode interactionMode_)
	{
		glm::vec4 data;
        if( hexExists( row, col ) )
			data = glm::vec4(grid_[ row * CHUNK_WIDTH + col ].getX(), grid_[ row * CHUNK_WIDTH + col ].getY(), 2, 0);
		
		int closest = 999;
		//check if in range of a blood vessel
		for( std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs )
		{
			//get distance
			int distance = getDistance((*bvs)->getRow(), (*bvs)->getCol(), row, col);
			BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
			//if it's closest encountered until now
			if(distance < closest)
			{
				closest = distance;
				//if in range
				if(distance <= bloodVessel->getRange())
				{				
					data.z = 0;//is in range
					data.w = distance * 1.0f/bloodVessel->getRange();//lerp factor
					data.w /= 1.5f;//rig value towards highlight tint
				}
				//if in large range
				else if(distance <= bloodVessel->getRange()*1.5)
				{
					data.z = 1;//is in large range
					data.w = (distance - bloodVessel->getRange()) * 1.0f/(bloodVessel->getRange()*1.5 - bloodVessel->getRange());//lerp factor
					data.w /= 3.0f;//rig value towards highlight tint
				}
			}
			switch(interactionMode_)
			{
				case InteractionMode::NORMAL:
				{
					if(cellSelected)
					{
						Cell* sel = (Cell*)(grid_[selectedPos.x * CHUNK_WIDTH + selectedPos.y].getNode());
						Hex* empty;
						getHex(row, col, &empty);
						
						int nextdoor = getDistance(selectedPos.x, selectedPos.y, row, col);
						if(nextdoor <= 1)
						{				
							if(sel->getState() == CellState::STEM)
							{
								if(sel->isInAlternateMode() && (empty->getType() == NodeType::EMPTY))
								{
									if(distance <= bloodVessel->getRange()*1.5)
									{				
										data.z = 3;//is empty and next to selected cell
										data.w = 0;
									}
								}
								else if((!sel->isInAlternateMode()) && ((empty->getType() == NodeType::EMPTY) || (empty->getType() == NodeType::CELL)) )
								{
									if(empty->getType() == NodeType::CELL)
									{
										Cell* notEmpty;
										notEmpty = (Cell*)(grid_[row * CHUNK_WIDTH + col].getNode());
										if(notEmpty->getState() == CellState::NORMAL)
										{
											if(distance <= bloodVessel->getRange()*1.5)
											{				
												data.z = 3;//is empty and next to selected cell
												data.w = 0;
											}
										}
									}
									else if(empty->getType() == NodeType::EMPTY)
									{
										if(distance <= bloodVessel->getRange()*1.5)
										{				
											data.z = 3;//is empty and next to selected cell
											data.w = 0;
										}
									}
								}
								
						
							}
							else if(empty->getType() == NodeType::EMPTY)
							{
								data.z = 3;//is empty and next to selected cell
								data.w = 0;
							}
							
						}
					
					}
					break;
				}
				case InteractionMode::BVCREATION:
				{
					 if(distance >= bloodVessel->getRange()*1.2 && score_ - T3E::SCORE::SPAWNED_BLOODVESSEL() > 0 )
					{
						if(!(data.z < 1))
						{
							data.z = 3;//is empty and next to selected cell
							data.w = 1.0f;
							data.w /= 1.5;							
						}
						
					}
					
					break;
				}
				case InteractionMode::KILLMODE:
				{
					// check to see if theres a cell in the hex
					if((hexExists( row, col )) && (grid_[row * CHUNK_WIDTH + col].getType() == NodeType::CELL))
					{
						// create a cell with the correct position
						Cell* killable = (Cell*)(grid_[row * CHUNK_WIDTH + col].getNode());

						// if it is a normal, arrested or a mutated cell highlight it
						if( ((killable->getState() == CellState::NORMAL) && (score_ - T3E::SCORE::KILLED_HEALTHY_CELL() > 0)) ||
							((killable->getState() == CellState::MUTATED) && (score_ - T3E::SCORE::KILLED_MUTATED_CELL() > 0)) ||
							((killable->getState() == CellState::ARRESTED) && (score_ - T3E::SCORE::KILLED_ARRESTED_CELL() > 0)) )
						{
							data.z = 3;//is killable
							data.w = 0;
						}
						break;
					}
					
				}
				default:
					// not a valid mode, cry
				break;				
			}
			
		}
		if(interactionMode_ == InteractionMode::BVCREATION)
		{
			if(score_ - T3E::SCORE::SPAWNED_BLOODVESSEL() >= 0)//enough points to set spawn
			{
				BloodVessel* bloodVessel = (BloodVessel*)(bloodVessels_[0]->getNode());
				if(closest > bloodVessel->getRange())//we're out of all bvs
				{
					//check for range of spawn points
					int lowest = 999;
					bool noSpawnPoints = true;
					for( std::vector<glm::vec2>::iterator sp = bvSpawnPoints_.begin(); sp != bvSpawnPoints_.end(); ++sp )
					{
						noSpawnPoints = false;
						int dist = getDistance(sp->x, sp->y, row, col);
						if(dist < lowest)
							lowest = dist;
					}
					
					if(noSpawnPoints)
					{
						data.z = 3;
						data.w = 0;
					}
					else if(lowest > bloodVessel->getRange())
					{
						data.z = 3;
						data.w = 0;
					}
				}
			}
		}
		
		if(data.z == 2)
			data.w = 1.0f;
		
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
	
	bool Grid::growBloodVesselAt( int row, int col)
	{
		T3E::Hex* neighbours[6];
		int adjacentCells = 0;

		// If the hex does not lie on the board or is not a cell, return error
        if( (!hexExists(row, col)) || (grid_[row * CHUNK_WIDTH + col].getType() != NodeType::CELL))
            return false;

		// return immediately if the growth coord is not a stem cell
		Cell* selectedCell = (Cell*)(grid_[row * CHUNK_WIDTH + col].getNode());
		if(selectedCell->getState() != CellState::STEM)
			return false;

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

		if( adjacentCells == 6 && score_ - T3E::SCORE::SPAWNED_BLOODVESSEL() > 0 )
		{
			newBloodVessel( row, col, nullptr );
			return true;
		}
	}
	
	int Grid::getDistance(int rowA, int colA ,int rowB, int colB)
	{
		//calculate the squared distance (avoid sqrt operation)
		int dSquared = (std::abs(colA - colB) + std::abs(rowA - rowB) + std::abs((-colA-rowA) - (-colB-rowB))) / 2;
		return dSquared;
	}
	
	bool Grid::setBvSpawn(int row, int col)
	{
		if(!hexExists( row, col ))
			return false;

		//get range from the first blood vessel of the bv vector(all have same range)
		int bvRange = ((BloodVessel*)(bloodVessels_[0]->getNode()))->getRange();
		
		//the new spawn point must be outside all existing blood vessels and spawn points ranges
		for( std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs )
		{
			BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
			if(inRange((*bvs)->getRow(), (*bvs)->getCol(), row, col, bvRange))
				return false;
		}
		for(std::vector<glm::vec2>::iterator sp = bvSpawnPoints_.begin(); sp != bvSpawnPoints_.end(); ++sp)
		{
			if(inRange(sp->x, sp->y, row, col, bvRange))
				return false;
		}	
		
		bvSpawnPoints_.push_back(glm::vec2(row, col));
        		
		return true;
	}
	
	glm::vec2 Grid::getBvSpawnCoords(int i)
	{
		glm::vec2 coords;
		//get world coords of the hex at that row and column
		coords.x = grid_[bvSpawnPoints_[i].x * CHUNK_WIDTH + bvSpawnPoints_[i].y].getX();
		coords.y = grid_[bvSpawnPoints_[i].x * CHUNK_WIDTH + bvSpawnPoints_[i].y].getY();
		return coords;
	}
}