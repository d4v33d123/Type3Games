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
{
}

MainGame::~MainGame()
{
}

void MainGame::createBloodVessel(int row, int column)
{
	//create a blood vessel
    //TODO: Memory Leak!!!!
    //This is overwriting the pointer whatever was in this position before without deleting it!
    delete grid_[row*COLUMNS + column];
	grid_[row*COLUMNS + column] = new T3E::BloodVessel();
	grid_[row*COLUMNS + column]->init(column, row, COLUMNS, ROWS);
	bloodVessels_.push_back(static_cast<T3E::BloodVessel*>(grid_[row*COLUMNS + column]));
	
	//loop trough neighbors and set their type to BLOOD_VESSEL
	//technically they will still remain cells
	//this is dirty af... but less overhead and more locality compared to creating 7 new pointers maybe is worth it?
	for(int n = 0; n < 6; ++n)
	{
		int nr, nc;//neighbor's row and column
		nr = grid_[row*COLUMNS + column]->getNeighbors()[n].row;
		nc = grid_[row*COLUMNS + column]->getNeighbors()[n].col;
		//not checking if coordinates are in range since this is done when checking neighbors of the centre hex
        delete grid_[nr * COLUMNS + nc];
		grid_[nr*COLUMNS + nc]->setType(T3E::Hex::BLOOD_VESSEL);
	}
	
	//now update all the cells in range to be affected???
	//or only affect cells that are born after the bv is created???
}

void MainGame::run()
{
	initSystems();
	
	//load sprites
	sprites_.push_back(new T3E::Sprite());
	sprites_.back()->init(-0.5f, -0.5f, 1.0f, 1.0f,"textures/cell.png");//x, y, width, height
	sprites_.push_back(new T3E::Sprite());
	sprites_.back()->init(-1.5f, -1.5f, 3.0f, 3.0f,"textures/bloodVessel.png");

	gameLoop();
}

void MainGame::initSystems()
{
	T3E::init();
	
	window_.create("Game Engine", screenWidth_, screenHeight_, T3E::BORDERLESS);

	//enable aplha blending	
	glEnable(GL_BLEND);//should we instead use frame buffer fetch in shader?
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);	
	
	//init camera at 0,0,1 looking at origin, up is y axis
	camera_.init(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f,0.0f,0.0f));
	camera_.setSensitivity(PAN_SENSITIVITY, ZOOM_SENSITIVITY);
	camera_.moveTo(glm::vec3(22.0f,12.5f,1.0f));//move to where stem cell is

	//init projection matrix
	//calculate aspect ratio
	window_.updateSizeInfo();//can do just once here since screen orientation is set to landscape always
	float ratio = float(window_.getScreenWidth())/float(window_.getScreenHeight());	
	projectionM_ = glm::perspective(90.0f, ratio, 0.1f, 100.0f);//fov 90°, aspect ratio, near and far clipping plane
	
	//fill grid with dead cells
	grid_.reserve(ROWS*COLUMNS);
	for(int r = 0; r < ROWS; ++r)
		for(int c = 0; c < COLUMNS; ++c)
		{
			grid_[r*COLUMNS + c] = new T3E::Cell();
			grid_[r*COLUMNS + c]->init(c, r, COLUMNS, ROWS);
		}	
	//create a stem cell in the middleish
	grid_[15 * COLUMNS + 15]->setType( T3E::Hex::STEM_CELL );
	cells_.push_back(static_cast<T3E::Cell*>(grid_[15*COLUMNS + 15]));

	createBloodVessel(19, 19);

	//init shaders
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
	
	/*CODE FOR TRIANGLES WITH PARALLAX
	//TRIANGLE PROGRAM
	// compile
	triangleProgram_.compileShaders("shaders/triangle_vs.txt", "shaders/triangle_ps.txt");
	// add attributes
	triangleProgram_.addAttribute("aPosition");
	triangleProgram_.addAttribute("aColour");
	// link
	triangleProgram_.linkShaders();
	// query uniform locations - could use "layout location" in shaders to set fixed locations
	triangle_finalM_location = triangleProgram_.getUniformLocation("finalM");*/
}

void MainGame::gameLoop()
{
	/*CODE FOR TRIANGLES WITH PARALLAX
	//would not put in main loop in actual implementation
	parallX = 0;
	parallY = 0; 
	
	//init triangle
	glGenBuffers(1, &triangleBufferName);
	
	//top right
	triangle[0].setPosition(0.0f, 0.5f);
	triangle[0].setColour(255, 153, 51, 255);
	//top left
	triangle[1].setPosition(-0.5f, -0.5f);
	triangle[1].setColour(0, 255, 255, 255);	
	//bottom left
	triangle[2].setPosition(0.5f, -0.5f);
	triangle[2].setColour(255, 0, 255, 255);
	
	glBindBuffer(GL_ARRAY_BUFFER, triangleBufferName);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
	
	//enable back face culling
	glEnable(GL_CULL_FACE);//GL_BACK is default value
	
	//our game loop
	while (gameState_ != GameState::EXIT)
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
		
		// print once every 10 frames
		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10)
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
	
	glDisable(GL_CULL_FACE);	
	
	//std::vector<T3E::Cell*>().swap(cells_); should we clear the vectors?	
	window_.destroy();//useful?
	SDL_Quit();
}

void MainGame::processInput()
{
	//TEST STUFF
	//for screen to world coord conversion
	glm::mat4 viewProjInverse;
	glm::vec4 worldPos;
    SDL_Point rowCol;
    int row, col;
	
	// processing our input
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			gameState_ = GameState::EXIT;
			break;
			
		case SDL_KEYDOWN:
			if(evnt.key.keysym.sym == SDLK_AC_BACK)//android back key
				gameState_ = GameState::EXIT;
			
			//EMULATOR ZOOM
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

            // Convert the world position to a row and column on the hex grid
            rowCol = world_to_grid( worldPos );
            row = rowCol.x;
            col = rowCol.y;
			
			//if in range
			if( ((row * COLUMNS + col) < (ROWS * COLUMNS )) && ((row * COLUMNS + col) >= 0) )
            {
                // TODO: Get davide to change the math, why is this offset required?
                T3E::Hex* hex = grid_[row * COLUMNS + col - 10];
                SDL_Log("Row %i col %i ", row, col);

                if( hex->getType() == T3E::Hex::NORMAL_CELL || hex->getType() == T3E::Hex::MUTATED_CELL || hex->getType() == T3E::Hex::CANCEROUS_CELL )
                {
                    T3E::Cell* cell = (T3E::Cell*)hex;

                    cell->makeGreen();
                }
                else if( hex->getType() == T3E::Hex::DEAD_CELL )
                {
                    // Set the cell to a blood vessle
                    createBloodVessel( row, col );
                }
            }
            
            // Draw cursor for debug purposes
            cursor_pos_ = touch_to_world( glm::vec2( evnt.tfinger.x, evnt.tfinger.y ) );
            
			break;
			
		case SDL_FINGERUP:
			--nOfFingers_;		
			/*HOW IN THE FUCKING HELL IS THIS AFFECTING FINGERDOWN?????
			viewProjInverse = projectionM_*viewM_;
			viewProjInverse = glm::inverse(viewProjInverse);
			worldPos.x = (evnt.tfinger.x*2.0f) - 1.0f;
			worldPos.y = 1 - (evnt.tfinger.y*2.0f);//y coord is inverted
			worldPos.z = 0.0f;
			worldPos.w = 1.0f;
			worldPos = viewProjInverse*worldPos; 	

			//SDL_Log("FINGERUP at world coord x: %f y: %f" , worldPos.x, worldPos.y);*/	

            // Reset the type of touch if the last finger was released
            if( nOfFingers_ == 0 ) finger_dragged_ = false;
			break;
			
		case SDL_FINGERMOTION:
            finger_dragged_ = true;

			//pan if only one finger is on screen; you don't want to pan during pinch motion
			if(nOfFingers_ < 2)
			{
				camera_.moveDelta(glm::vec3(-evnt.tfinger.dx, evnt.tfinger.dy, 0.0f));
				
/* 				//parallax test
				parallX -= evnt.tfinger.dx*2;
				parallY += evnt.tfinger.dy*2; */
			}

			break;
		
		case SDL_MULTIGESTURE: 		
			//pinch zoom
			camera_.zoom(-evnt.mgesture.dDist);
			break;
			
		default:
			break;
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

	
	/*CODE FOR TRIANGLES WITH PARALLAX
	//RENDER TRIANGLE x 3 at different locations
	triangleProgram_.use();
	for(int i = -1; i < 2; ++i)
	{
		//send matrix to shaders
		//translate world matrix to separate triangles and create parallax
		glm::mat4 transM = glm::translate(worldM, glm::vec3(parallX + i, parallY + i, 0.0f));
		finalM = finalM*transM;//shold be just worldM but whatever, it's a test
		glUniformMatrix4fv(triangle_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM));
		// bind the buffer object
		glBindBuffer(GL_ARRAY_BUFFER, triangleBufferName);
		// tell opengl that we want to use the first attribute array
		glEnableVertexAttribArray(0);
		// This is our position attribute pointer, last value is the byte offset before the value is used in the struct
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, position));
		// this is our pixel attribute pointer;
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, colour));
		//this is out UV attribute pointer;
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(T3E::Vertex), (void*)offsetof(T3E::Vertex, uv));
		// draw our 6 verticies
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// disable the vertex attrib array
		glDisableVertexAttribArray(0);
		// unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		//reset matrix
		finalM = projectionM*viewM*worldM;
	}
	triangleProgram_.stopUse();	
	*/
		
	//RENDER CELLS
	cellProgram_.use();
	
	//blood vessels
	for(int i = 0; i < bloodVessels_.size(); ++i)
	{
		//move to hex position
		worldM_ = glm::translate(worldM_, glm::vec3(bloodVessels_[i]->getX(), bloodVessels_[i]->getY(), 0.0f));
		finalM_ = projectionM_*viewM_*worldM_;
		
		//send matrix to shaders
		glUniformMatrix4fv(cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_));
		//set tint
		float tint[] = {bloodVessels_[i]->getTint().x , bloodVessels_[i]->getTint().y , bloodVessels_[i]->getTint().z, bloodVessels_[i]->getTint().w};
		glUniform4fv(inputColour_location, 1, tint);
		//set texture	
		//glActiveTexture(GL_TEXTURE0 + grid_[current].getType());
		glActiveTexture(GL_TEXTURE0+1);
		glUniform1i(sampler0_location, 0);
		//sprites_[grid_[current].getType()]->draw();
		sprites_[1]->draw();
		
		//reset matrices
		//make an identityM object to reset instead of creating new one a bagillion times?
		//or having to fetch other non local obj even worse?
		worldM_ = glm::mat4();
		finalM_ = projectionM_ * viewM_ * worldM_;
	}
	
	//cells
	for(int i = 0; i < cells_.size(); ++i)
	{
        // Don't render cells that are part of a blood vessle
        if( cells_[i]->getType() == T3E::Hex::BLOOD_VESSEL ) continue;

		//move to hex position
		worldM_ = glm::translate(worldM_, glm::vec3(cells_[i]->getX(), cells_[i]->getY(), 0.0f));
		finalM_ = projectionM_*viewM_*worldM_;
		
		//send matrix to shaders
		glUniformMatrix4fv(cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_));
		//set tint
		float tint[] = {cells_[i]->getTint().x , cells_[i]->getTint().y , cells_[i]->getTint().z, cells_[i]->getTint().w};
		glUniform4fv(inputColour_location, 1, tint);
		//set texture	
		//glActiveTexture(GL_TEXTURE0 + grid_[current].getType());
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(sampler0_location, 0);
		//sprites_[grid_[current].getType()]->draw();
		sprites_[0]->draw();
		
		//reset matrices
		//make an identityM object to reset instead of creating new one a bagillion times?
		//or having to fetch other non local obj even worse?
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
        float tint[] = { 1.0f, 1.0f, 1.0f, 1.0f };
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

glm::vec4 MainGame::touch_to_world( glm::vec2 touch_coord )
{
    glm::vec4 result( touch_coord.x, touch_coord.y, 0.0f, 1.0f );

    result.x = result.x * 2.0f - 1.0f;
    result.y = result.y * 2.0f - 1.0f;
    result.y *= -1.0f; // Invert to match OpenGL coords
    
    //SDL_Log("Touch at NDC: %f %f", result.x, result.y );

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

    //SDL_Log("World coord: %f %f", result.x, result.y);
    return result;
}

SDL_Point MainGame::world_to_grid( glm::vec4 world_coord )
{
	float fracCol, fracRow, colD, rowD, zD, fracZ;
	int col, row, z;
	glm::mat2 layout_;//inverse of pointy top layout matrix

    //world coord to hex coord
    layout_ = glm::mat2(sqrt(3.0f) / 3.0f, -1.0f / 3.0f, 0.0f, 2.0f / 3.0f);
    
    fracCol = world_coord.x/0.54f;
    fracCol = layout_[0][0] * fracCol + layout_[0][1] * fracRow;
    
    fracRow = world_coord.y/0.54;
    fracRow = layout_[1][0] * fracCol + layout_[1][1] * fracRow;
    
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
        z = -row-col;

    

    return SDL_Point{ row, col };
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
