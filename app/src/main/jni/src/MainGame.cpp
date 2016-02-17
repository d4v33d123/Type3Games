#include "MainGame.h"

MainGame::MainGame() : 
	screenHeight_(800),
	screenWidth_(600),
	time_(0.0f), 
	gameState_(GameState::PLAY),
	maxFPS_(60.0f),
	nOfFingers_(0),
	PAN_SENSITIVITY(6.0f),
	ZOOM_SENSITIVITY(6.0f),
    finger_dragged_(false),
	pressTimer_(0),
	fingerPressed_(false),
	cellSelected_(false)
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
    grid_.newCell( 5, 5, T3E::CellState::STEM, 0, nullptr );

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
		
		time_ += 0.1f;
		calculateFPS();			
		if(grid_.update(frameTime_))
			cellSelected_ = false;
		renderGame();
		processInput(frameTime_);
				
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

void MainGame::processInput(float dTime)
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
			
			fingerPressed_ = true;
			//record position in case we need it later
			pressPos_ = glm::vec2(evnt.tfinger.x, evnt.tfinger.y);
			
            // Convert the touch position to a world position
            worldPos = touch_to_world( glm::vec2( evnt.tfinger.x, evnt.tfinger.y ) );
                        
            // Draw cursor for debug purposes
            cursor_pos_ = touch_to_world( glm::vec2( evnt.tfinger.x, evnt.tfinger.y ) );
			break;
			
		case SDL_FINGERUP:
			--nOfFingers_;
			
			fingerPressed_ = false;			
			pressTimer_ = 0;
            
			// Only spawn cells when the last finger is lifted,
            // AND the cursor was not moved
            if( nOfFingers_ == 0 && finger_dragged_ == false )
            {
                // convert the touch to a world position
                worldPos = touch_to_world( glm::vec2( evnt.tfinger.x, evnt.tfinger.y ) );
                // convert the world pos to a grid row column
                rowCol = world_to_grid( worldPos );
                
				//try to spawn a blood vessel
				growBloodVesselAt( rowCol.x, rowCol.y );
				
				//if a cell was selected
				if(cellSelected_)
				{
					//try to spawn
					grid_.spawnCell(selectedPos_.x, selectedPos_.y, rowCol.x, rowCol.y);
					
					grid_.unselectCell(selectedPos_.x, selectedPos_.y);
					cellSelected_ = false;					
				}
				
				//try to select a cell
				//also, if a new cell was created, select it
				selectCell(rowCol.x, rowCol.y);
            }

            // Reset the type of touch if the last finger was released
            if( nOfFingers_ == 0 ) finger_dragged_ = false;
			break;
			
		case SDL_FINGERMOTION:
            finger_dragged_ = true;
			fingerPressed_ = false;
			
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
	
	//check for finger pressure
	if(fingerPressed_)
	{
		pressTimer_ += dTime;
		//SDL_Log("%f", pressTimer_);
		if(pressTimer_ >= 1000)
		{
			pressTimer_ = 0;
			fingerPressed_ = false;
			rowCol = world_to_grid(touch_to_world(pressPos_));
			//try to arrest
			grid_.arrestCell(rowCol.x, rowCol.y);
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
		//TODO: we don't need to tint blood vessels; remove this and make new shader that doesn't use tint? 
		float tint[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glUniform4fv(inputColour_location, 1, tint);
		
        //use texture 1
		glActiveTexture(GL_TEXTURE0+1);
		glUniform1i(sampler0_location, 1);
		sprites_[1]->draw();
		
		//reset matrices
		worldM_ = glm::mat4();
		finalM_ = projectionM_ * viewM_ * worldM_;
	}
	
	//cells
	for(int i = 0; i < grid_.numCells(); ++i)
	{
		T3E::Cell* current = (T3E::Cell*)grid_.getCell(i)->getNode();
		
		// move to hex position
		worldM_ = glm::translate( worldM_, glm::vec3( grid_.getCell(i)->getX(), grid_.getCell(i)->getY(), 0.0f ) );
		finalM_ = projectionM_ * viewM_ * worldM_;
		
		// send matrix to shaders
		glUniformMatrix4fv( cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_) );

		// set tint
		float tint[] = {current->getTint().x ,current->getTint().y , current->getTint().z, current->getTint().w};
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

bool MainGame::growBloodVesselAt( int row, int col )
{
    T3E::Hex* neighbours[6];
    int adjacentCells = 0;
    //int adjacentBlood = 0;

    // return immidiately if the growth coord is not avalible
    if( !grid_.isEmpty( row, col ) ) return false;

    // Check the cell has a live neighbour
    if( grid_.getNeighbours( row, col, neighbours ) )
    {
        // Count the number of adjacent cells
        for( int i = 0; i < 6; i++ )
        {
            if( neighbours[i] != nullptr )
            {
				if(neighbours[i]->getType() == T3E::NodeType::CELL)
					adjacentCells++;
				else
					return false;
				/* 
                switch( neighbours[i]->getType() )
                {
                    case T3E::NodeType::CELL:
                        adjacentCells++;
                        break;

                    case T3E::NodeType::BLOOD_VESSEL_CORE:
                    case T3E::NodeType::BLOOD_VESSEL_EDGE:
                        adjacentBlood++;
                        break;
                        
                    default: continue;
                } */
            }
			else
				return false;
        }
    }

    /* if( (adjacentCells > 0 && adjacentCells < 6) || adjacentBlood > 0 )
    {
        grid_.newCell( row, col, T3E::CellState::STEM, 0, nullptr );
    } */
    if( adjacentCells == 6 )
    {
        grid_.newBloodVessel( row, col, nullptr );
		return true;
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

bool MainGame::selectCell(int row, int col)
{
	//if the coordinates contain a cell
	if(grid_.selectCell(row, col))
	{
		cellSelected_ = true;
		selectedPos_ = glm::vec2(row, col);
		return true;
	}
	return false;
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
