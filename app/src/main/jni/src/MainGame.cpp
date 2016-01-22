#include "MainGame.h"

MainGame::MainGame() : 
	screenHeight_(800),
	screenWidth_(600),
	time_(0.0f), 
	gameState_(GameState::PLAY),
	maxFPS_(60.0f)
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
	
	//TRIANGLE PROGRAM
	// compile
	triangleProgram_.compileShaders("shaders/triangle_vs.txt", "shaders/triangle_ps.txt");
	// add attributes
	triangleProgram_.addAttribute("aPosition");
	triangleProgram_.addAttribute("aColour");
	// link
	triangleProgram_.linkShaders();
	// query uniform locations - could use "layout location" in shaders to set fixed locations
	triangle_finalM_location = triangleProgram_.getUniformLocation("finalM");
}

void MainGame::gameLoop()
{
	// TOUCH TEST STUFF
	camPos.x = 0.0f;
	camPos.y = 0.0f;
	camPos.z = 1.0f;
	fingers = 0;	
	worldPos.x = 0.0f;
	worldPos.y = 0.0f;
	worldPos.z = 0.0f;
	//END TOUCH TEST STUFF
	//parallax test
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
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//init view Matrix
	//camera at 0,0,1 looking at origin, up is y axis
	viewM = glm::lookAt(glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
	//init projection Matrix
	//calculate aspect ratio
	window_.updateSizeInfo();//can do just once here since screen orientation is set to landscape always
	float ratio = float(window_.getScreenWidth())/float(window_.getScreenHeight());
	//fov 90°, ratio, near and far clipping plane
	projectionM = glm::perspective(90.0f, ratio, 0.1f, 100.0f);
	
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
	
	window_.destroy();//useful?
	SDL_Quit();
}

void MainGame::processInput()
{
	//TOUCH TEST STUFF
	glm::mat4 viewProjInverse;
	
	//END TOUCH TEST STUFF
	
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
				camPos = camPos + glm::vec3(0.0f,0.0f,-0.1f);
				viewM = glm::lookAt(camPos, lookatPos, glm::vec3(0.0f,1.0f,0.0f));
			}
			if(evnt.key.keysym.sym == SDLK_x)//zoom out
			{
				camPos = camPos + glm::vec3(0.0f,0.0f,0.1f);
				viewM = glm::lookAt(camPos, lookatPos, glm::vec3(0.0f,1.0f,0.0f));
			}
			
			break;
			
		//TOUCH TEST STUFF
		case SDL_FINGERDOWN:
			++fingers;
			//SDL_Log("fingers: %d", fingers);
			
			//IS THIS RIGHT? seems to go wrong when camera is moved from center
			//get world coordinates of touch position
			viewProjInverse = projectionM*viewM;
			viewProjInverse = glm::inverse(viewProjInverse);
			worldPos.x = (evnt.tfinger.x*2.0f) - 1.0f;
			worldPos.y = 1 - (evnt.tfinger.y*2.0f);//y coord is inverted
			worldPos.z = 0.0f;
			worldPos.w = 1.0f;
			worldPos = viewProjInverse*worldPos;
			SDL_Log("FINGERDOWN at world coord x: %f y: %f" , worldPos.x, worldPos.y);
			
			break;
		case SDL_FINGERUP:
			--fingers;
			//SDL_Log("fingers: %d", fingers);
			
			//IS THIS RIGHT? seems to go wrong when camera is moved from center
			//get world coordinates of touch position
			viewProjInverse = projectionM*viewM;
			viewProjInverse = glm::inverse(viewProjInverse);
			worldPos.x = (evnt.tfinger.x*2.0f) - 1.0f;
			worldPos.y = 1 - (evnt.tfinger.y*2.0f);//y coord is inverted
			worldPos.z = 0.0f;
			worldPos.w = 1.0f;
			worldPos = viewProjInverse*worldPos;
			SDL_Log("FINGERUP at world coord x: %f y: %f" , worldPos.x, worldPos.y);
			
			break;
			
		case SDL_FINGERMOTION:
			if(fingers < 2)
			{
				camPos = camPos + glm::vec3(-evnt.tfinger.dx*3, evnt.tfinger.dy*3, 0.0f);
				lookatPos = camPos;
				lookatPos.z = 0.0f;
				viewM = glm::lookAt(camPos, lookatPos, glm::vec3(0.0f,1.0f,0.0f));
				
				//parallax test
				parallX -= evnt.tfinger.dx*2;
				parallY += evnt.tfinger.dy*2;
			}
			break;
		
		case SDL_MULTIGESTURE:
			//SDL_Log("%f" , float(evnt.mgesture.dDist));
			camPos = camPos + glm::vec3(0.0f,0.0f,-evnt.mgesture.dDist*4);
			viewM = glm::lookAt(camPos, lookatPos, glm::vec3(0.0f,1.0f,0.0f));
			break;
			
		//END TOUCH TEST STUFF
			
		default:
			break;
		}
	}
}

void MainGame::renderGame()
{
	//rendering our game
	glClearDepthf(1.0);
	//clear both buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//multiply world, view and projection matrices together
	//order matters!
	finalM = projectionM*viewM*worldM;

	//RENDER TRIANGLE x 3 at different locations
	triangleProgram_.use();
	for(int i = -1; i < 2; ++i)
	{
		//send matrix to shaders
		//translate world matrix to separate triangles and create parallax
		glm::mat4 transM = glm::translate(worldM, glm::vec3(parallX + i, parallY + i, 0.0f));
		finalM = finalM*transM;//souhld be just worldM but whatever, it's a test
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
		
	//RENDER CELL
	cellProgram_.use();
	//enable aplha blending
	//should we take it out of the loop if all our scene is blended? should we instead use frame buffer fetch(for blending at least)?
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);	
	//send matrix to shaders
	glUniformMatrix4fv(cell_finalM_location, 1, GL_FALSE, glm::value_ptr(finalM));
	//send a colour to tint the texture
	float tint[] = {1.0f ,parallX, -parallY, 1.0f};//randomish stuff to make it change with cam movement
	glUniform4fv(inputColour_location, 1, tint);
	//draw sprites with texture 0
	glActiveTexture(GL_TEXTURE0);	
	glUniform1i(sampler0_location, 0);
	for (int i = 0; i < sprites_.size(); i++)
	{
		// could do glActiveTexture(GL_TEXTURE0 + i) or similar to change texture
		sprites_[i]->draw();
	}	
	//disable aplha blending
	glDisable(GL_BLEND);	
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