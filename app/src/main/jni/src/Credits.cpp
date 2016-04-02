#include "Credits.h"

command Credits::run(T3E::window* window, T3E::AudioEngine* audioEngine)
{
	maxFPS_ = 60.0f;
	
	window_ = window;
	audioEngine_ = audioEngine;
	
	initSystems();
		
	buttonPress_ = audioEngine_->loadSoundEffect("sound/Player_CellDivide_Move.ogg");
	
	command result = gameLoop();
	
	return result;
}

void Credits::initSystems()
{	
	//init ortho matrix
	//inverting top with bottom to avoid sprites being drawn upside down
	//note that this will put origin at bottom left, while screen coords have origin at top left
	window_->updateSizeInfo();
	orthoM_ = glm::ortho(0.0f, float( window_->getScreenWidth() ), 0.0f, float( window_->getScreenHeight() ));

	backButton_.init(10.0f, 10.0f,
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/7.0f, "textures/ui2.png",
		1/4.0f, 1.0f,
		3/4.0f, 0.0f,
		3/4.0f, 0.0f);	

		//background sprite
	backgroundSprite_.init(0.0f, 0.0f, float(window_->getScreenWidth()), float(window_->getScreenHeight()),"textures/credits.png", 0, 0, 1.0f, 1.0f);
	
	
	//init shaders
	initShaders();
}

void Credits::initShaders()
{
	 //CELL PRORGAM
	// compile
	tintedSpriteProgram_.compileShaders("shaders/tintedSprite_vs.txt", "shaders/tintedSprite_ps.txt");
	// add attributes
	tintedSpriteProgram_.addAttribute("aPosition");
	tintedSpriteProgram_.addAttribute("aColour");
	tintedSpriteProgram_.addAttribute("aTexCoord");
	// link
	tintedSpriteProgram_.linkShaders();
	// query uniform locations - could use "layout location" in shaders to set fixed locations
	tintedSprite_finalM_location = tintedSpriteProgram_.getUniformLocation("finalM");
	sampler0_location = tintedSpriteProgram_.getUniformLocation("sampler0");
	inputColour_location = tintedSpriteProgram_.getUniformLocation("inputColour");
}

command Credits::gameLoop()
{	
	command c;
	Uint32 old_ticks = 0;
	Uint32 ticks = 0;
	
	while((c = processInput()) == command::NONE)
	{
		float startTicks = SDL_GetTicks();
		renderGame();
		
		float frameTicks = SDL_GetTicks() - startTicks;
		//Limit the FPS to the max FPS
		if (1000.0f / maxFPS_ > frameTicks)
		{
			SDL_Delay(1000.0f / maxFPS_ - frameTicks);
		}

		old_ticks = ticks;
		ticks = SDL_GetTicks();
	}
	
	return c;
}

command Credits::processInput()
{
	glm::vec2 screenCoords;
	
	// processing our input
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		switch( evnt.type )
		{
		case SDL_QUIT:
			return command::QUIT;
			break;
		
		case SDL_FINGERDOWN:
			//get touch pos in screen coordinates for UI interaction
			//invert y to match our ortho projection (origin at bottom left for ease of life)
			screenCoords = glm::vec2(evnt.tfinger.x * float(window_->getScreenWidth()),
				float(window_->getScreenHeight()) - evnt.tfinger.y * float(window_->getScreenHeight()));
				
			if(backButton_.touchCollides(screenCoords))
			{
				backButton_.press();
			}
			return command::NONE;
			break;
			
		case SDL_FINGERUP:
			//get touch pos in screen coordinates for UI interaction
			//invert y to match our ortho projection (origin at bottom left for ease of life)
			screenCoords = glm::vec2(evnt.tfinger.x * float(window_->getScreenWidth()),
				float(window_->getScreenHeight()) - evnt.tfinger.y * float(window_->getScreenHeight()));
				
			if(backButton_.touchCollides(screenCoords))
			{
				backButton_.unpress();
				return command::MENU;	
			}
			return command::NONE;
			break;
			
		default:
			return command::NONE;
		}
	}	
}

void Credits::renderGame()
{
	//clear both buffers
	glClearDepthf(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	tintedSpriteProgram_.use();
	
	//RENDER BACKGROUND
	// send ortho matrix to shaders
	glUniformMatrix4fv( tintedSprite_finalM_location, 1, GL_FALSE, glm::value_ptr(orthoM_) );
	float bgtint[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glUniform4fv(inputColour_location, 1, bgtint);
	// set texture	
	GLint texid = T3E::ResourceManager::getTexture("textures/background.png").unit;
	glActiveTexture(GL_TEXTURE0 + texid);	
	glUniform1i(sampler0_location, texid);
	//draw sprite
	backgroundSprite_.draw();
	
	//RENDER UI	
	// send ortho matrix to shaders
	glUniformMatrix4fv( tintedSprite_finalM_location, 1, GL_FALSE, glm::value_ptr(orthoM_) );		
	float tint[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glUniform4fv(inputColour_location, 1, tint);
	// set texture
	texid = T3E::ResourceManager::getTexture("textures/ui.png").unit;
	glActiveTexture(GL_TEXTURE0 + texid);	
	glUniform1i(sampler0_location, texid);
	//draw sprite
	backButton_.draw();
		
	tintedSpriteProgram_.stopUse();	

	// swap our buffers 
	window_->swapBuffer();
}

void Credits::calculateFPS()
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