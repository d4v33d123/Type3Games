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
			
		/* fill grid with cells to test stuff 
		for(int row = 0; row < CHUNK_WIDTH; ++row)
			for(int col = 0; col < CHUNK_WIDTH; ++col)
				newCell(row, col, CellState::MUTATED, 5, nullptr);
		*/
    }

    Grid::~Grid()
    {
		for(int row = 0; row < CHUNK_WIDTH; ++row)
			for(int col = 0; col < CHUNK_WIDTH; ++col)
				setEmpty(row,col);
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
	
	bool Grid::getHex(int row, int col, Hex** hex)
	{
		 // If the hex does not lie on the board, return error
        if( !hexExists( row, col ) )
            return false;

        // get that hex
        *hex = &(grid_[ row * CHUNK_WIDTH + col ]);

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
        		addScore( T3E::SCORE::SPAWNED_STEM_CELL() );
        	break;
        	case CellState::NORMAL:
        		addScore( T3E::SCORE::SPAWNED_HEALTHY_CELL() );
        	break;
        	case CellState::MUTATED:
        		addScore( T3E::SCORE::SPAWNED_MUTATED_CELL() );
        	break;
        	case CellState::CANCEROUS:
        		addScore( T3E::SCORE::SPAWNED_CANCER_CELL() );
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

        addScore( T3E::SCORE::SPAWNED_BLOODVESSEL() );

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
	        		addScore( T3E::SCORE::KILLED_HEALTHY_CELL() );
	        	break;
	        	case CellState::MUTATED:
	        		addScore( T3E::SCORE::KILLED_MUTATED_CELL() );
	        	break;
	        	case CellState::CANCEROUS:
	        		addScore( T3E::SCORE::KILLED_CANCER_CELL() );
	        	break;
	        	case CellState::ARRESTED:
	        		addScore( T3E::SCORE::KILLED_ARRESTED_CELL() );
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

            addScore( T3E::SCORE::KILLED_BLOODVESSEL() );
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
		// TODO: Doesn't range also need to be squared?!
		int dSquared = (std::abs(colA - colB) + std::abs(rowA - rowB) + std::abs((-colA-rowA) - (-colB-rowB))) / 2;
		return dSquared <= range;
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
            if( distToBv <= bloodvessel_range )
			{
				death_chance = cancerDeathChance_;
			}
			else
			{
				death_chance = clampPercnt(cancerDeathChance_ * (distToBv - bloodvessel_range));
			}
		}
		else
		{
			if( distToBv <= 2 )
			{
				// The hex is adjacent to the blood vessel
				death_chance = adjacentBloodvesselDeathChance_;
			}
			else if( distToBv <= bloodvessel_range )
			{
				// take account of adjacent cells are two away from the blood vessel
				float lerpAmount = (distToBv - 2) / float(bloodvessel_range - 2);

				// As the cell gets farther away from the blood vessel
				// Lerp from the adjacent_death_chance to the far_death_chance
				death_chance = (adjacentBloodvesselDeathChance_ * (1.0f - lerpAmount)) + (farBloodvesselDeathChance_ * lerpAmount);
			}
			else
			{
				// If the cell is outside the range of a blood vessel
				// It's death chance is based on it's parents + some value
				death_chance = parentDchance + childDeathChanceIncrease_;
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
	
		//update bvs
		for( std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs )
		{
			BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
			bloodVessel->update(dTime);
		}
		
		//try to create blood vessels at spawn points
		for(std::vector<glm::vec2>::iterator sp = bvSpawnPoints_.begin(); sp != bvSpawnPoints_.end();)
		{
			if(growBloodVesselAt(sp->x, sp->y, &selectedCellDied))
			{
				sp = bvSpawnPoints_.erase(sp);
				playVessel_ = true;
			}
			else
				++sp;
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
					current->update(dTime);//do this after isDead() check so we don't skip rendering the last frame
			}
			//update cell, check if it's time to split
			else if(current->update(dTime) && (! current->isSelected() ))
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
							bool reallyEmpty = true;
							//check if we already selected this hex for a new cell in this update loop
							for(std::vector<birthInfo>::iterator c = newCells.begin(); c != newCells.end(); ++c)
							{
								if(neighbours[lucky]->getRow() == c->row && neighbours[lucky]->getCol() == c->col)
									reallyEmpty = false;
							}
							if(reallyEmpty)
							{
								current->split(lucky);//signal for split animation
								
								int random_val = rand() & 255;
								
								// create a new cell depending on current's type
								switch(current->getState())
								{
								case CellState::STEM:
									newCells.push_back( birthInfo(
										neighbours[lucky]->getRow(),
										neighbours[lucky]->getCol(),
										CellState::STEM,
										current->getDeathChance(),
                                        false));

									break;								
								case CellState::NORMAL:
									if( random_val < (chanceOfMutation_ / 100.0f) * 255 )
									{
										newCells.push_back( birthInfo(
											neighbours[lucky]->getRow(),
											neighbours[lucky]->getCol(),
											CellState::MUTATED,
											current->getDeathChance(),
                                            true));
									}
									else
									{
										newCells.push_back( birthInfo(
											neighbours[lucky]->getRow(),
											neighbours[lucky]->getCol(),
											CellState::NORMAL,
											current->getDeathChance(),
                                            false));
									}
									break;								
								case CellState::MUTATED:
									if( random_val < (chanceOfCancer_ / 100.0f) * 255 )
									{
										newCells.push_back( birthInfo(
											neighbours[lucky]->getRow(),
											neighbours[lucky]->getCol(),
											CellState::CANCEROUS,
											current->getDeathChance(),
                                            true));
									}
									else
									{
										newCells.push_back( birthInfo(
											neighbours[lucky]->getRow(),
											neighbours[lucky]->getCol(),
											CellState::MUTATED,
											current->getDeathChance(),
                                            false));
									}
									break;								
								case CellState::CANCEROUS: // Cancerous cells always spawn more cancerous cells
									newCells.push_back( birthInfo(
										neighbours[lucky]->getRow(),
										neighbours[lucky]->getCol(),
										CellState::CANCEROUS,
										current->getDeathChance(),
                                        false));
									break;								
								default:
									break;
								}
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
						if(current->isSelected())
						{
							selectedCellDied = true;
							current->unselect();
						}
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
            Cell* stemChild;
			// If it's parent was a stem cell, it gets the minimum death chance
			if( c->state == CellState::STEM )
			{				
				newCell( c->row, c->col, CellState::NORMAL, 0, &stemChild );
				stemChild->setDeathChance( minDeathChance_ );
			}
			else
			{
				newCell( c->row, c->col, c->state, c->parentDeathChance, &stemChild );
                if(c->showChange)
                    stemChild->showChangeOn();
			}
		}
		
		return selectedCellDied;
	}
	
	bool Grid::arrestCell(int row, int col, bool* cellSelected)
	{
		// If the hex does not lie on the board or is not a cell, return error
        if( (!hexExists( row, col )) || (grid_[row * CHUNK_WIDTH + col].getType() != NodeType::CELL))
		{
			return false;
		}
            
		Cell* cell = (Cell*)(grid_[row * CHUNK_WIDTH + col].getNode());
		if(cell->isSplitting() || cell->isInCreation() || cell->isDying()) return false;//can't arrest in those cases!
		
		//only arrest normal cells
		if(cell->getState() == CellState::NORMAL && currency_ + T3E::SCORE::ARRESTED_CELL() >= 0 )
		{
			if(cell->isSelected())
			{
				*cellSelected = false;
				cell->unselect();
			}				
			cell->arrest();
			addScore( T3E::SCORE::ARRESTED_CELL() );
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
		
		if(selectedCell->isSplitting() || selectedCell->isInCreation()) return false;//can't split while splitting or creating!
		
		//a stem cell must be in alternate mode to spawn
		if(selectedCell->getState() == CellState::STEM)
			if(!selectedCell->isInAlternateMode())
				return false;
		
		//don't spawn if arrested
		//this could happen if you:
		//	-select a cell and then hold down to arrest it
		//	-without lifting your finger, slightly move it of a small amount that doesn't trigger fingerDragged but moves coordinates to nearby hex
		//	-now lift the finger and you're trying to spawn from an arrested cell
		// *** FIXED *** added call to Cell::unselect in Grid::arrestCell() and added notification to MainGame so not necessary anymore
		// leaving this here since it's a tricky bug and could happen with future changes
		if(selectedCell->getState() == CellState::ARRESTED) return false;
		
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
						// spawn new cell, pass the selected cells dc as it's parent's
						if(newCell(touchRow, touchCol, selectedCell->getState(), selectedCell->getDeathChance(), nullptr))
						{
							selectedCell->split(i);
							
							//now we increase parent's death chance aswell if it was normal
							if(selectedCell->getState() == CellState::NORMAL)
								// TODO: why is incDeathChance even a method? shoud be set( get + inc )
								//because incdeathchance also checks the increase doesn't add up
								//to over 99 (is it still relevant to current gameplay rules?)
								selectedCell->incDeathChance( parentDeathChanceIncrease_ );
                            
                            if(selectedCell->getState() == CellState::STEM)
                                selectedCell->stemToStemOn();//use stem to stem animation
							
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
		
		if(cell->isSplitting() || cell->isInCreation() || cell->isDying()) return false;
		
		//different costs for normal and mutated cells
		if( cell->getState() == CellState::NORMAL && currency_ - T3E::SCORE::KILLED_HEALTHY_CELL() >= 0 )
		{
			cell->kill();
			return true;
		}
		else if( cell->getState() == CellState::MUTATED && currency_ - T3E::SCORE::KILLED_MUTATED_CELL() >= 0 )
		{
			cell->kill();
			return true;
		}
		else if( cell->getState() == CellState::ARRESTED && currency_ - T3E::SCORE::KILLED_ARRESTED_CELL() >= 0 )
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
				
		if(selectedCell->getState() != CellState::STEM) return false;//not a stem cell selected, return error
		if(selectedCell->isSplitting() || selectedCell->isInCreation()) return false;//don't move while splitting or creating!
		if(selectedCell->isInAlternateMode())
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
					
					//if it's empty, spawn a new stem cell there
					if(isEmpty(touchRow, touchCol))
					{
						//check if is in the range of at least one blood vessel
						for( std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs )
						{
							BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
							if(inRange((*bvs)->getRow(), (*bvs)->getCol(), touchRow, touchCol, bloodVessel->getRange()*1.5f))
							{
								T3E::Cell* stem;
								if(newCell(touchRow, touchCol, CellState::STEM, 0, &stem))
								{
									stem->ignoreBirthDelay();//we're not splitting, so no split animation stuff
									stem = nullptr;
									//delete the old stem cell
									setEmpty(selRow, selCol);
									return true;
								}							
								//couldn't create cell
								return false;
							}
						}
					}
					else if(cell->getState() == CellState::NORMAL && !cell->isSplitting() && !cell->isInCreation())
					{
						int dc = cell->getDeathChance();
						glm::vec4 tint = cell->getTint();
						//check if is in the range of at least one blood vessel
						for( std::vector<Hex*>::iterator bvs = bloodVessels_.begin(); bvs != bloodVessels_.end(); ++bvs )
						{
							BloodVessel* bloodVessel = (BloodVessel*)((*bvs)->getNode());
							if(inRange((*bvs)->getRow(), (*bvs)->getCol(), touchRow, touchCol, bloodVessel->getRange()*1.5f))
							{
								T3E::Cell* theNewCell;
								
								setEmpty(touchRow, touchCol);
								if(newCell(touchRow, touchCol, CellState::STEM, 0, &theNewCell))
								{
									theNewCell->ignoreBirthDelay();//stem
									//move the normal cell to the stem cell's new pos
									newCell( selRow, selCol, CellState::NORMAL, dc, &theNewCell);
									theNewCell->ignoreBirthDelay();
									theNewCell->hardcodeNormalTint(tint);
									theNewCell = nullptr;
									return true;
								}				
								// couldn't move the stem cell so put the normal cell back 
								newCell( touchRow, touchCol, CellState::NORMAL, dc, &theNewCell);
								theNewCell->ignoreBirthDelay();
								theNewCell->hardcodeNormalTint(tint);
								theNewCell = nullptr;
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
						if(!(sel->isSplitting() || sel->isInCreation()))//can't split in those cases
						{
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
					}
					break;
				}
				case InteractionMode::BVCREATION:
				{
					 if(distance >= bloodVessel->getRange()*1.2 && currency_ - T3E::SCORE::SPAWNED_BLOODVESSEL() >= 0 )
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
						
						if(!(killable->isSplitting() || killable->isInCreation() || killable->isDying()))//can't kill in those cases
							// if it is a normal, arrested or a mutated cell highlight it
							if( ((killable->getState() == CellState::NORMAL) && (currency_ - T3E::SCORE::KILLED_HEALTHY_CELL() >= 0)) ||
								((killable->getState() == CellState::MUTATED) && (currency_ - T3E::SCORE::KILLED_MUTATED_CELL() >= 0)) ||
								((killable->getState() == CellState::ARRESTED) && (currency_ - T3E::SCORE::KILLED_ARRESTED_CELL() >= 0)) )
							{
								data.z = 3;//is killable
								data.w = 0;
							}		
					}
					break;
				}
				default:
					// not a valid mode, cry
				break;				
			}
			
		}
		if(interactionMode_ == InteractionMode::BVCREATION)
		{
			if(currency_ - T3E::SCORE::SPAWNED_BLOODVESSEL() >= 0)//enough points to set spawn
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
	
	bool Grid::growBloodVesselAt( int row, int col, bool* selectedCellDied)
	{
		bool selectedCellInvolved = false;
		T3E::Hex* neighbours[6];

		// If the hex does not lie on the board or is not a cell, return error
        if( (!hexExists(row, col)) || (grid_[row * CHUNK_WIDTH + col].getType() != NodeType::CELL))
            return false;

		// return immediately if the growth coord is not a stem cell
		Cell* selectedCell = (Cell*)(grid_[row * CHUNK_WIDTH + col].getNode());
		if(selectedCell->getState() != CellState::STEM)
			return false;
		
		if(selectedCell->isSplitting() || selectedCell->isInCreation()) return false;//don't create while animating

		if(currency_ - T3E::SCORE::SPAWNED_BLOODVESSEL() <= 0 ) return false;// not enough points

		if(!(moreStems())) return false; // not enough stem cells 
		
		if( getNeighbours( row, col, neighbours ) )
		{
			// Count the number of adjacent cells
			for( int i = 0; i < 6; i++ )
			{
				if( neighbours[i] != nullptr )
				{
					if(neighbours[i]->getType() == T3E::NodeType::CELL)
					{
						selectedCell = (Cell*)(grid_[neighbours[i]->getRow() * CHUNK_WIDTH + neighbours[i]->getCol()].getNode());
						if(selectedCell->isSplitting() || selectedCell->isInCreation() || selectedCell->isDying() || (selectedCell->getState() == T3E::CellState::STEM)) return false;//don't create while animating or if there is another stem cell involved
						if(selectedCell->isSelected())//must notify or grid highlight will remain
							selectedCellInvolved = true;
					}
					else
						return false;//not a cell
				}
				else
					return false;//not existing					
			}
		}

		if(newBloodVessel( row, col, nullptr ))
		{
			if(selectedCellInvolved)
				*selectedCellDied = true;
			return true;
		}
		return false;
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

		for(std::vector<glm::vec2>::iterator sp = bvSpawnPoints_.begin(); sp != bvSpawnPoints_.end(); )
		{
			if(sp->x == row, sp->y == col)
			{
				sp = bvSpawnPoints_.erase(sp);
				return true;
			}
			else
			{
				++sp;
			}
		}

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

	void Grid::addScore( int score )
	{
		if( score > 0 )
			high_score_ += score;

		currency_ += score;
	}
	
	bool Grid::moreStems()
	{
		// loop through all of the cells and count to at least 2, if more than  one is found return true, if not return false
		int numStems = 0;
		Cell* selectedCell;
		for(std::vector<Hex*>::iterator it = cells_.begin(); it != cells_.end(); ++it)
		{
			Hex* iter = (*it);
			selectedCell = (Cell*)(grid_[iter->getRow() * CHUNK_WIDTH + iter->getCol()].getNode());
			if(selectedCell->getState() == T3E::CellState::STEM)
			{
				numStems++;
			}
			if(numStems > 1)
			{
				return true;
			}
		}
		return false;
	}
}
