#include "MainGame.h"

MainGame::MainGame() : 
	screenHeight_(800),
	screenWidth_(600),
	time_(0.0f), 
	gameState_(GameState::PLAY),
	maxFPS_(60.0f),
	nOfFingers_(0),
	ROWS(30),
	COLUMNS(30),
	PAN_SENSITIVITY(6.0f),
	ZOOM_SENSITIVITY(6.0f)
{
}

MainGame::~MainGame()
{
}

void MainGame::run()
{
	initSystems();
	
	//load sprites
	sprites_.push_back(new T3E::Sprite());
	//x, y, width, height
	sprites_.back()->init(-0.5f, -0.5f, 1.0f, 1.0f,"textures/cell.png");

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
			grid_[r*COLUMNS + c] = T3E::Cell(c, r, COLUMNS, ROWS);	
	
	//make a stem cell in the middleish
	grid_[15*COLUMNS + 15].changeType(T3E::Cell::STEM);
	cells_.push_back(&(grid_[15*COLUMNS + 15]));
	
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
					int c = cells_[i]->hex().getNeighbors()[lucky].col;
					int r = cells_[i]->hex().getNeighbors()[lucky].row;		
					
					//if neighbor position is valid (not out of bounds)
					if(cells_[i]->hex().getNeighbors()[lucky].row != -1)
					{
						//if neighbor position is an empty space (dead cell)
						if(grid_[r*COLUMNS + c].getType() == T3E::Cell::DEAD)
						{
							//randomise split time
							cells_[i]->newSplitTime();
							
							//rolls
							int noMutation;
							int noCancer;
							
							//create a new cell depending on current's type
							switch(cells_[i]->getType())
							{
							//NORMAL
							case T3E::Cell::NORMAL:
								//increase parent death chance by 5%
								cells_[i]->incDeathChance(5);
								//roll for mutation
								noMutation = rand()%100;
								if(noMutation)
								{
									grid_[r*COLUMNS + c].changeType(T3E::Cell::NORMAL, cells_[i]->getDeathChance());
								}
								
								else
								{
									grid_[r*COLUMNS + c].changeType(T3E::Cell::MUTATED, cells_[i]->getDeathChance());
								}
								break;
							
							//STEM
							case T3E::Cell::STEM:
								grid_[r*COLUMNS + c].changeType(T3E::Cell::NORMAL, cells_[i]->getDeathChance());
								break;
								
							//MUTATED
							case T3E::Cell::MUTATED:
								//increase parent death chance by 5%
								cells_[i]->incDeathChance(5);
								//roll for cancer
								noCancer = rand()%100;
								if(noCancer)
								{
									grid_[r*COLUMNS + c].changeType(T3E::Cell::MUTATED, cells_[i]->getDeathChance());
								}
								else
								{
									grid_[r*COLUMNS + c].changeType(T3E::Cell::CANCEROUS);
								}
								break;
								
							//CANCEROUS
							case T3E::Cell::CANCEROUS:
								grid_[r*COLUMNS + c].changeType(T3E::Cell::CANCEROUS);
								break;
								
							default:
								break;
							}
							
							//add the new cell to the living cells vector
							cells_.push_back(&(grid_[r*COLUMNS + c]));
						}
					}							
				}
				else//die
				{
					cells_[i]->changeType(T3E::Cell::DYING);
				}
			}
		}
		
		//remove dead cells; could put in previus loop
		int i = 0;
		while (i < cells_.size())
		{
			if (cells_[i]->getType() == T3E::Cell::DYING)
			{
				cells_[i]->changeType(T3E::Cell::DEAD);
				cells_.erase( cells_.begin() + i );
			}
			else 
				++i;
		}
		
		// print once every 10 frames... OR NOT! huehuehuehue
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
	//for world to hex coord conversion
	float fracCol, fracRow, colD, rowD, zD, fracZ;
	int col, row, z;
	glm::mat2 layout_;//inverse of pointy top layout matrix
	//END TEST STUFF
	
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
			//get world coordinates of touch position -> NOT WORKING
			/* 			
			worldPos.x = evnt.tfinger.x;
			worldPos.y = evnt.tfinger.y;
			
			worldPos.x = (worldPos.x * 2.0f) - 1.0f;
			worldPos.y = 1.0f - (worldPos.y * 2.0f);//y coord is inverted
			worldPos.z = 0.0f;
			worldPos.w = 1.0f;
			viewProjInverse = projectionM_*viewM_;
			viewProjInverse = glm::inverse(viewProjInverse);
			worldPos = viewProjInverse * worldPos;
			
			// worldPos.x = worldPos.x/worldPos.w;
			// worldPos.y = worldPos.y/worldPos.w;
			
			// worldPos.x = 0.0f;
			// worldPos.y = 0.0f;
			SDL_Log("FINGERDOWN at world coord x: %f y: %f" , worldPos.x, worldPos.y);
			
			//world coord to hex coord -> WORKING
			layout_ = glm::mat2(sqrt(3.0f) / 3.0f, -1.0f / 3.0f, 0.0f, 2.0f / 3.0f);
			
			fracCol = worldPos.x/0.54f;
			fracCol = layout_[0][0] * fracCol + layout_[0][1] * fracRow;
			
			fracRow = worldPos.y/0.54;
			fracRow = layout_[1][0] * fracCol + layout_[1][1] * fracRow;
			
			fracZ = -fracCol-fracRow;
			
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
			
			//if in range
			if(((row*columns + col) < (rows*columns))&&((row*columns + col) >= 0))
				grid_[row*columns + col].makeBlue();
			// for(int r = 0; r < rows; ++r)
				// for(int c = 0; c < columns; ++c)
				// {
					// grid_[15].makeBlue();
				// }
			*/
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
			break;
			
		case SDL_FINGERMOTION:
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
	
	for(int i = 0; i < cells_.size(); ++i)
	{
		//move to hex position
		int current = cells_[i]->hex().getR()*COLUMNS + cells_[i]->hex().getC();
		
		worldM_ = glm::translate(worldM_, glm::vec3(grid_[current].hex().getX(), grid_[current].hex().getY(), 0.0f));
		finalM_ = projectionM_*viewM_*worldM_;
		
		//send matrix to shaders
		glUniformMatrix4fv(cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM_));
		//set tint
		float tint[] = {grid_[current].getTint().x , grid_[current].getTint().y , grid_[current].getTint().z, grid_[current].getTint().w};
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
	
	cellProgram_.stopUse();

	// swap our buffers 
	window_.swapBuffer();
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