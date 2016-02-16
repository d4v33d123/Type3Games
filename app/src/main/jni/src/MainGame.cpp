#include "MainGame.h"

MainGame::MainGame() : 
	screenHeight_(800),
	screenWidth_(600),
	time_(0.0f), 
	gameState_(GameState::PLAY),
	maxFPS_(60.0f),
	nOfFingers_(0),
	ROWS(50),
	COLUMNS(50),
	PAN_SENSITIVITY(6.0f),
	ZOOM_SENSITIVITY(6.0f),
    finger_dragged_(false)
{}

MainGame::~MainGame()
{
}

void MainGame::run()
{
	initSystems();
	
	//load sprites
	sprites_.push_back( new T3E::Sprite() );
	sprites_.back()->init(-0.5f, -0.5f, 1.0f, 1.0f,"textures/cell.png"); // x, y, width, height
	sprites_.push_back( new T3E::Sprite() );
	sprites_.back()->init(-1.5f, -1.5f, 3.0f, 3.0f,"textures/bloodVessel.png");

	gameLoop();
}

void MainGame::initSystems()
{
	T3E::init();
	
	window_.create("Game Engine", screenWidth_, screenHeight_, T3E::BORDERLESS);

	// enable aplha blending	
	glEnable( GL_BLEND );//should we instead use frame buffer fetch in shader?
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	
	// init camera at 0,0,1 looking at origin, up is y axis
	camera_.init( glm::vec3( 0.0f, 0.0f, 1.0f ), glm::vec3( 0.0f,0.0f,0.0f ) );
	camera_.setSensitivity( PAN_SENSITIVITY, ZOOM_SENSITIVITY );
	camera_.moveTo(glm::vec3( 5.0f, 5.0f, 2.0f ) );

	// init projection matrix
	// calculate aspect ratio
	window_.updateSizeInfo(); // can do just once here since screen orientation is set to landscape always
	float ratio = float( window_.getScreenWidth() )/float( window_.getScreenHeight() );	
	projectionM_ = glm::perspective( 90.0f, ratio, 0.1f, 100.0f ); // fov 90°, aspect ratio, near and far clipping plane
	        
    // Set the first cell
    grid_.newCell( 5, 5, nullptr );

    // Set a test blood vessel
    grid_.newBloodVessel( 7, 7, nullptr );

	// init shaders
	initShaders();
}

void MainGame::initShaders()
{
	//CELL PRORGAM
	// compile
	cellProgram_.compileShaders("shaders/cell_vs.txt", "shaders/cell_ps.txt");
	// add attributes
	cellProgram_.addAttribute("aPosition");
	cellProgram_.addAttribute("aColour");
	cellProgram_.addAttribute("aTexCoord");
	// link
	cellProgram_.linkShaders();
	// query uniform locations - could use "layout location" in shaders to set fixed locations
	inputColour_location = cellProgram_.getUniformLocation("inputColour");
	cell_finalM_location = cellProgram_.getUniformLocation("finalM");
	sampler0_location = cellProgram_.getUniformLocation("sampler0");
}

void MainGame::gameLoop()
{
	//enable back face culling
	glEnable(GL_CULL_FACE);//GL_BACK is default value
	
	//our game loop
	while( gameState_ != GameState::EXIT )
	{
		// used for frame time measuring
		float startTicks = SDL_GetTicks();

		processInput();
		time_ += 0.1f;
		renderGame();
		calculateFPS();			
		
		/*
		//TEMPORARY TEST CODE
		//SELF SPAWNING BLOOD VESSELS
		for(int pos = 0; pos < (ROWS*COLUMNS); ++pos)
		{
			//if cell is empty
			if(grid_[pos]->getType() == T3E::Hex::DEAD_CELL)
			{
				int cellCount = 0;
				int nr, nc;//neighbor's row and column
				
				//check each neighbor has a cell in it
				for(int nbr = 0; nbr < 6; ++nbr)
				{
					nr = grid_[pos]->getNeighbors()[nbr].row;
					nc = grid_[pos]->getNeighbors()[nbr].col;
					//if nbr pos is valid
					if(nr != -1)
					{
						T3E::Hex::type t = grid_[nr*COLUMNS + nc]->getType();
						//if there's a cell inc counter
						if(t != T3E::Hex::DEAD_CELL && t != T3E::Hex::BLOOD_VESSEL)
							++cellCount;
						else
							break;
					}
				}
				//if there's a hex ring
				if(cellCount == 6)
				{
					//kill cells in the ring
					for(int nbr = 0; nbr < 6; ++nbr)
					{
						nr = grid_[pos]->getNeighbors()[nbr].row;
						nc = grid_[pos]->getNeighbors()[nbr].col;
						grid_[nr*COLUMNS + nc]->setType(T3E::Hex::DYING_CELL);
						//remove dead cells
						int i = 0;
						while (i < cells_.size())
						{
							if (cells_[i]->getType() == T3E::Hex::DYING_CELL)
							{
								cells_[i]->setType(T3E::Hex::DEAD_CELL);
								cells_.erase( cells_.begin() + i );
							}
							else 
								++i;
						}
					}
					//create bv in centre
					createBloodVessel(pos/COLUMNS, pos%COLUMNS);
				}					
			}
		}
		*/
		
		/*
		//for each living cell
		for(int i = 0; i < cells_.size(); ++i)
		{
			//if it's time to split
			if(cells_[i]->update(frameTime_))
			{
				//roll for chance to die
				int die = rand()%100;			
				//if no death
				if(die >= cells_[i]->getDeathChance())
				{
					//pick a lucky neighboring hex
					int lucky = rand()%6;
					int c = cells_[i]->getNeighbors()[lucky].col;
					int r = cells_[i]->getNeighbors()[lucky].row;		
					
					//if neighbor position is valid (not out of bounds)
					if(cells_[i]->getNeighbors()[lucky].row != -1)
					{
						//if neighbor position is an empty space (dead cell)
						if(grid_[r*COLUMNS + c]->getType() == T3E::Hex::DEAD_CELL)
						{
							//randomise split time
							cells_[i]->newSplitTime();
							
							//rolls
							int noMutation;
							int noCancer;
							
							//create a new cell depending on current's type
							switch(cells_[i]->getType())
							{
							//STEM
							case T3E::Hex::STEM_CELL:
								grid_[r*COLUMNS + c]->setType(T3E::Hex::NORMAL_CELL, cells_[i]->getDeathChance() + 5);
								break;
								
							//NORMAL
							case T3E::Hex::NORMAL_CELL:
								//increase parent death chance by 5%
								cells_[i]->incDeathChance(5);
								//roll for mutation
								noMutation = rand()%100;
								if(noMutation && grid_[r * COLUMNS + c]->getType() != T3E::Hex::BLOOD_VESSEL)
								{
									grid_[r*COLUMNS + c]->setType(T3E::Hex::NORMAL_CELL, cells_[i]->getDeathChance());
								}
								
								else if( grid_[r * COLUMNS + c]->getType() != T3E::Hex::BLOOD_VESSEL )
								{
									grid_[r*COLUMNS + c]->setType(T3E::Hex::MUTATED_CELL, cells_[i]->getDeathChance());
								}
								break;
								
							//MUTATED
							case T3E::Hex::MUTATED_CELL:
								//increase parent death chance by 5%
								cells_[i]->incDeathChance(5);
								//roll for cancer
								noCancer = rand()%100;
								if(noCancer && grid_[r * COLUMNS + c]->getType() != T3E::Hex::BLOOD_VESSEL )
								{
									grid_[r*COLUMNS + c]->setType(T3E::Hex::MUTATED_CELL, cells_[i]->getDeathChance());
								}
								else if( grid_[r * COLUMNS + c]->getType() != T3E::Hex::BLOOD_VESSEL );
								{
									grid_[r*COLUMNS + c]->setType(T3E::Hex::CANCEROUS_CELL);
								}
								break;
								
							//CANCEROUS
							case T3E::Hex::CANCEROUS_CELL:
								grid_[r*COLUMNS + c]->setType(T3E::Hex::CANCEROUS_CELL);
								break;
								
							default:
								break;
							}
							
							//add the new cell to the living cells vector
							cells_.push_back(static_cast<T3E::Cell*>(grid_[r*COLUMNS + c]));
							//check if it's in range of a blood vessel
							for(int bvs = 0; bvs < bloodVessels_.size(); ++bvs)
							{
								if(bloodVessels_[bvs]->inRange(cells_.back()->getC(), cells_.back()->getR(), bloodVessels_[bvs]->getRange()))
								{
									//cells_.back()->makeGreen();
									//reset death chance
									cells_.back()->setDeathChance(0);
								}
							}
						}
					}							
				}
				else//die
				{
					cells_[i]->setType(T3E::Hex::DYING_CELL);
				}
			}
		}
		*/
			
		//remove dead cells
        /*
		int i = 0;
		while (i < cells_.size())
		{
			if (cells_[i]->getType() == T3E::Hex::DYING_CELL)
			{
				cells_[i]->setType(T3E::Hex::DEAD_CELL);
				cells_.erase( cells_.begin() + i );
			}
			else 
				++i;
		}*/
		
		// print once every 10 frames
		static int frameCounter = 0;
		frameCounter++;
		if( frameCounter == 10 )
		{
			//SDL_Log("%f\n", _fps);
			frameCounter = 0;
		}

		float frameTicks = SDL_GetTicks() - startTicks;
		//Limit the FPS to the max FPS
		if (1000.0f / maxFPS_ > frameTicks)
		{
			SDL_Delay(1000.0f / maxFPS_ - frameTicks);
		}	
	}
	
	glDisable( GL_CULL_FACE );
		
	window_.destroy(); // useful?
	SDL_Quit();
}

void MainGame::processInput()
{
	glm::vec4 worldPos;
    SDL_Point rowCol;
    int row, col;
    T3E::Hex* neighbours;
	
	// processing our input
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		switch( evnt.type )
		{
		case SDL_QUIT:
			gameState_ = GameState::EXIT;
			break;
			
		case SDL_KEYDOWN:
			if(evnt.key.keysym.sym == SDLK_AC_BACK) // android back key
				gameState_ = GameState::EXIT;
			
			// EMULATOR ZOOM
			if(evnt.key.keysym.sym == SDLK_z)//zoom in
			{
				camera_.zoom(-0.05f);
			}
			if(evnt.key.keysym.sym == SDLK_x)//zoom out
			{
				camera_.zoom(0.05f);
			}
			
			break;
			
		case SDL_FINGERDOWN:
			++nOfFingers_;
			
            // Convert the touch position to a world position
            worldPos = touch_to_world( glm::vec2( evnt.tfinger.x, evnt.tfinger.y ) );
                        
            // Draw cursor for debug purposes
            cursor_pos_ = touch_to_world( glm::vec2( evnt.tfinger.x, evnt.tfinger.y ) );
            
			break;
			
		case SDL_FINGERUP:
			--nOfFingers_;		

            // Only spawn cells when the last finger is lifted,
            // AND the cursor was not moved
            if( nOfFingers_ == 0 && finger_dragged_ == false )
            {
                // convert the touch to a world position
                worldPos = touch_to_world( glm::vec2( evnt.tfinger.x, evnt.tfinger.y ) );
                // convert the world pos to a grid row column
                rowCol = world_to_grid( worldPos );
                
                tryNewCell( rowCol.x, rowCol.y );
            }

            // Reset the type of touch if the last finger was released
            if( nOfFingers_ == 0 ) finger_dragged_ = false;
			break;
			
		case SDL_FINGERMOTION:
            finger_dragged_ = true;

			// pan if only one finger is on screen; you don't want to pan during pinch motion
			if( nOfFingers_ < 2 )
			{
				camera_.moveDelta( glm::vec3(-evnt.tfinger.dx, evnt.tfinger.dy, 0.0f) );
			}

			break;
		
		case SDL_MULTIGESTURE: 		
			// pinch zoom
			camera_.zoom( -evnt.mgesture.dDist );
			break;
			
		default: break;
		}
	}
}

void MainGame::renderGame()
{
	//clear both buffers
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//update matrices
	viewM_ = glm::lookAt(camera_.getPosition(), camera_.getLookAt(), camera_.getUp());
	finalM_ = projectionM_*viewM_*worldM_;//order matters!
		
	//RENDER CELLS
	cellProgram_.use();
	
	//blood vessels
	for(int i = 0; i < grid_.numBloodVessels(); ++i)
	{
		//move to hex position
		worldM_ = glm::translate( worldM_, glm::vec3( grid_.getBloodVessel(i)->getX(), grid_.getBloodVessel(i)->getY(), 0.0f ) );
		finalM_ = projectionM_ * viewM_ * worldM_;
		
		//send matrix to shaders
		glUniformMatrix4fv(cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_));
		//set tint
		//float tint[] = {bloodVessels_[i]->getTint().x , bloodVessels_[i]->getTint().y , bloodVessels_[i]->getTint().z, bloodVessels_[i]->getTint().w};
        float tint[] = { 1.0f, 0.2f, 0.2f, 1.0f };
		glUniform4fv(inputColour_location, 1, tint);
		
        //set texture	
		glActiveTexture(GL_TEXTURE0+1);
		glUniform1i(sampler0_location, 0);
		sprites_[1]->draw();
		
		//reset matrices
		worldM_ = glm::mat4();
		finalM_ = projectionM_ * viewM_ * worldM_;
	}
	
	//cells
	for(int i = 0; i < grid_.numCells(); ++i)
	{
		// move to hex position
		worldM_ = glm::translate( worldM_, glm::vec3( grid_.getCell(i)->getX(), grid_.getCell(i)->getY(), 0.0f ) );
		finalM_ = projectionM_ * viewM_ * worldM_;
		
		// send matrix to shaders
		glUniformMatrix4fv( cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_) );

		// set tint
		//float tint[] = {cells_[i]->getTint().x , cells_[i]->getTint().y , cells_[i]->getTint().z, cells_[i]->getTint().w};
        float tint[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glUniform4fv(inputColour_location, 1, tint);

		// set texture	
        glActiveTexture(GL_TEXTURE0);
		glUniform1i(sampler0_location, 0);
		sprites_[0]->draw();
		
		// reset matrices
		// make an identityM object to reset instead of creating new one a bagillion times?
		// or having to fetch other non local obj even worse?
		worldM_ = glm::mat4();
		finalM_ = projectionM_*viewM_*worldM_;
	}

    // Cursor
    {
        // Move to cursor position
        worldM_ = glm::translate( worldM_, glm::vec3( cursor_pos_.x, cursor_pos_.y, 0.0f) );
        finalM_ = projectionM_ * viewM_ * worldM_;

        // Sent matrix to shaders
        glUniformMatrix4fv( cell_finalM_location, 1, GL_FALSE, glm::value_ptr( finalM_ ) );

        // Set tint
        float tint[] = { 0.0f, 0.0f, 0.5f, 0.2f };
        glUniform4fv( inputColour_location, 1, tint );

        // Set texture
        glActiveTexture( GL_TEXTURE0 );
        glUniform1i( sampler0_location, 0 );
        
        sprites_[0]->draw();

        // Reset matrices
        worldM_ = glm::mat4();
        finalM_ = projectionM_ * viewM_ * worldM_;
    }
		
	cellProgram_.stopUse();

	// swap our buffers 
	window_.swapBuffer();
}

void MainGame::tryNewCell( int row, int col )
{
    T3E::Hex* neighbours[6];

    // Check the cell has a live neighbour
    if( grid_.getNeighbours( row, col, neighbours ) )
    {
        // Check at least one of the neighbours is alive
        for( int i = 0; i < 6; i++ )
        {
            if( neighbours[i] != nullptr )
            {
                if( neighbours[i]->getType() == T3E::NodeType::CELL )
                {
                    grid_.newCell( row, col, nullptr );
                }
            }
        }
    }
}

glm::vec4 MainGame::touch_to_world( glm::vec2 touch_coord )
{
    glm::vec4 result( touch_coord.x, touch_coord.y, 0.0f, 1.0f );

    result.x = result.x * 2.0f - 1.0f;
    result.y = result.y * 2.0f - 1.0f;
    result.y *= -1.0f; // Invert to match OpenGL coords
    
    // Calculate the inverse matrix of the view and projection
    viewProjInverse = glm::inverse( projectionM_ * viewM_ );

    // multiply the touch position by that
    result = viewProjInverse * result;
    
    // divide by w
    result.x /= result.w;
    result.y /= result.w;

    // scale the position to account for zoom
    result.x = camera_.getPosition().x + ( result.x - camera_.getPosition().x ) * result.w * camera_.getPosition().z;
    result.y = camera_.getPosition().y + ( result.y - camera_.getPosition().y ) * result.w * camera_.getPosition().z;

    SDL_Log("World coord: %f %f", result.x, result.y);
    return result;
}

SDL_Point MainGame::world_to_grid( glm::vec4 world_coord )
{
	float fracCol, fracRow, fracZ;//fractional coordinates
	int col, row, z;//final coordinates
	float colD, rowD, zD;//difference between fractional and rounded coords
	
    glm::mat2 layoutInverse_= glm::mat2(sqrt(3.0f) / 3.0f, -1.0f / 3.0f, 0.0f, 2.0f / 3.0f);
	fracCol = (layoutInverse_[0][0] * world_coord.x + layoutInverse_[0][1] * world_coord.y) / 0.54f;
	fracRow = (layoutInverse_[1][0] * world_coord.x + layoutInverse_[1][1] * world_coord.y) / 0.54f;

	fracZ = -fracCol -fracRow;
    
    col = round(fracCol);
    row = round(fracRow);
    z = round(fracZ);
    
	colD = abs(col - fracCol);
    rowD = abs(row - fracRow);
    zD = abs(z - fracZ);
    
    if((colD > rowD) && (colD > zD))
    {
        col = -row-z;
    }
    else if(rowD > zD)
    {
        row = -col-z;
    }
    else
	{
        z = -row-col;
	}

    return SDL_Point{ row, col};
}

void MainGame::calculateFPS()
{
	static const int NUM_SAMPLES = 1000;
	static float frameTimes[NUM_SAMPLES];
	static int currentFrame = 0;
	static float prevTicks = SDL_GetTicks();

	float currentTicks;
	currentTicks = SDL_GetTicks();

	frameTime_ = currentTicks - prevTicks;
	
	//avoid bug that set frameTime_ to very low negative number on first calculation
	if(frameTime_ < 0)
		frameTime_ = 0;
	
	frameTimes[currentFrame % NUM_SAMPLES] = frameTime_;

	// set previous ticks to new ticks now
	prevTicks = currentTicks;

	currentFrame++;

	int count;

	if (currentFrame < NUM_SAMPLES)
	{
		count = currentFrame;
	}
	else
	{
		count = NUM_SAMPLES;
	}

	float frameTimeAverage = 0;
	for (int i = 0; i < count; i++)
	{
		frameTimeAverage += frameTimes[i];
	}
	frameTimeAverage /= count;

	if (frameTimeAverage > 0)
	{
		fps_ = 1000.0 / frameTimeAverage;
	}
	else
	{
		fps_ = 60.0f;
	}
}
