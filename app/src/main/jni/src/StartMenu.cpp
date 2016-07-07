#include "StartMenu.h"
#include "Type3Engine/TextRenderer.h"
#include "Type3Engine/window.h"

StartMenu::StartMenu() :
finger_down_(false),
finger_lifted_(false)
{}

StartMenu::~StartMenu()
{}

command StartMenu::run(T3E::window* window, T3E::AudioEngine* audioEngine, T3E::TextRenderer* TextRenderer )
{
	maxFPS_ = 60.0f;
	
	window_ = window;
	audioEngine_ = audioEngine;
	textRenderer_ = TextRenderer;
	
	initSystems();
	
	buttonPress_ = audioEngine_->loadSoundEffect("sound/Player_CellDivide_Move.ogg");

	command result = gameLoop();

	return result;
}

void StartMenu::initSystems()
{	    
	//init ortho matrix
	//inverting top with bottom to avoid sprites being drawn upside down
	//note that this will put origin at bottom left, while screen coords have origin at top left
	window_->updateSizeInfo();
	orthoM_ = glm::ortho(0.0f, float( window_->getScreenWidth() ), 0.0f, float( window_->getScreenHeight() ));
	
	//top right
	playButton_.init(float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())*(7.0f/9.0f),
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/9.0f, "textures/ssheet0.png",
		1.0f/16, 1.0f/4,
		0.0f, 2.0f/4,
		0.0f, 3/4.0f);
	tutorialButton_.init(float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())*(5.0f/9.0f),
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/9.0f, "textures/ssheet0.png",
		1.0f/16, 1.0f/4,
		1.0f/16, 2.0f/4,
		1.0f/16, 3/4.0f);
		
	creditsButton_.init(float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())*(3.0f/9.0f),
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/9.0f, "textures/ssheet0.png",
		1.0f/16, 1.0f/4,
		3.0f/16, 2.0f/4,
		3.0f/16, 3/4.0f);
		
	quitButton_.init(float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())*(1.0f/9.0f),
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/9.0f, "textures/ssheet0.png",
		1.0f/16, 1.0f/4,
		5.0f/16, 2.0f/4,
		5.0f/16, 3/4.0f);

	//background sprite
	backgroundSprite_.init(0.0f, 0.0f, float(window_->getScreenWidth()), float(window_->getScreenHeight()),"textures/background.png", 0, 0, 1.0f, 1.0f);
	    
	//init shaders
	initShaders();
}

void StartMenu::initShaders()
{
	 //CELL PRORGAM
	// compile
	tintedSpriteProgram_.compileShaders("shaders/tintedSprite_vs.txt", "shaders/tintedSprite_ps.txt");
	// add attributes
	tintedSpriteProgram_.addAttribute("aPosition");
	//tintedSpriteProgram_.addAttribute("aColour");
	tintedSpriteProgram_.addAttribute("aTexCoord");
	// link
	tintedSpriteProgram_.linkShaders();
	// query uniform locations - could use "layout location" in shaders to set fixed locations
	tintedSprite_finalM_location = tintedSpriteProgram_.getUniformLocation("finalM");
	sampler0_location = tintedSpriteProgram_.getUniformLocation("sampler0");
	inputColour_location = tintedSpriteProgram_.getUniformLocation("inputColour");
}

command StartMenu::gameLoop()
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
	
	if(c != command::QUIT && c != command::CREDITS)//no load screen for credits and quitting
		renderLoadScreen();
	
	audioEngine_ = nullptr;//TESTING
	return c;
}

command StartMenu::processInput()
{
	finger_lifted_ = false;
	command c = command::NONE;
	
	// processing our input
	SDL_Event evnt;
	while( SDL_PollEvent(&evnt) )
	{
		switch( evnt.type )
		{
		case SDL_QUIT:
			return command::QUIT;

		break;
		case SDL_FINGERMOTION: // WARNING: fall through
		case SDL_FINGERDOWN:
			finger_down_ = true;
			
			//get touch pos in screen coordinates for UI interaction
			//invert y to match our ortho projection (origin at bottom left for ease of life)
			finger_position_pixels_.x = evnt.tfinger.x * float(window_->getScreenWidth());
			finger_position_pixels_.y = window_->getScreenHeight() - evnt.tfinger.y * float(window_->getScreenHeight());

		break;
		case SDL_FINGERUP:
			//get touch pos in screen coordinates for UI interaction
			//invert y to match our ortho projection (origin at bottom left for ease of life)
			finger_position_pixels_.x = evnt.tfinger.x * float(window_->getScreenWidth());
			finger_position_pixels_.y = window_->getScreenHeight() - evnt.tfinger.y * float(window_->getScreenHeight());
			
			finger_down_ = false;
			finger_lifted_ = true;

		break;			
		default:
		break;
		}
	}

	// Depress buttons when the finger is over them
	if( finger_down_ )
	{
		playButton_.touchCollides(finger_position_pixels_)? playButton_.press() : playButton_.unpress();
		tutorialButton_.touchCollides(finger_position_pixels_)? tutorialButton_.press() : tutorialButton_.unpress();
		creditsButton_.touchCollides(finger_position_pixels_)? creditsButton_.press() : creditsButton_.unpress();
		quitButton_.touchCollides(finger_position_pixels_)? quitButton_.press() : quitButton_.unpress();
	}

	// Activate buttons when the finger is lifted on them
	if( finger_lifted_ )
	{
		if( playButton_.touchCollides(finger_position_pixels_) )
		{
			c = command::PLAY;
		}

		if( tutorialButton_.touchCollides(finger_position_pixels_) )
		{
			c = command::TUTORIAL;
		}

		if( creditsButton_.touchCollides(finger_position_pixels_) )
		{
			c = command::CREDITS;
		}

		if( quitButton_.touchCollides(finger_position_pixels_) )
		{
			c = command::QUIT;
		}

		playButton_.unpress();
		tutorialButton_.unpress();
		creditsButton_.unpress();
		quitButton_.unpress();
	}

	return c;	
}

void StartMenu::renderGame()
{
	// clear both buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	tintedSpriteProgram_.use();
	
	/* RENDER BACKGROUND */
	// send ortho matrix to shaders
	glUniformMatrix4fv( tintedSprite_finalM_location, 1, GL_FALSE, glm::value_ptr(orthoM_) );
	float bgtint[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glUniform4fv( inputColour_location, 1, bgtint );
	// set texture
	GLint texid = T3E::ResourceManager::getTexture("textures/background.png").unit;
	glActiveTexture( GL_TEXTURE0 + texid );
	glUniform1i( sampler0_location, texid );
	// Draw the backgorund sprite
	backgroundSprite_.draw();
	
	/* RENDER UI */
	// send ortho matrix to shaders
	glUniformMatrix4fv( tintedSprite_finalM_location, 1, GL_FALSE, glm::value_ptr(orthoM_) );		
	float tint[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glUniform4fv( inputColour_location, 1, tint );

	// draw menu buttons
	playButton_.draw();
	tutorialButton_.draw();
	creditsButton_.draw();
	quitButton_.draw();
	
	tintedSpriteProgram_.stopUse();	

	// swap our buffers 
	window_->swapBuffer();
}

void StartMenu::renderLoadScreen()
{
	//clear both buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	tintedSpriteProgram_.use();
	
	//RENDER BACKGROUND
	// send ortho matrix to shaders
	glUniformMatrix4fv( tintedSprite_finalM_location, 1, GL_FALSE, glm::value_ptr(orthoM_) );
	float bgtint[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glUniform4fv( inputColour_location, 1, bgtint );
	// Draw the background sprite
	backgroundSprite_.draw();
	
	tintedSpriteProgram_.stopUse();	

    textRenderer_->putString( "... Loading ...", -0.75f, 0.2, 0.1f );
    textRenderer_->render();
    
	// swap our buffers 
	window_->swapBuffer();
	
	SDL_Delay(500);//wait a few milllisecs, don't want to just flash this for a millisecond if loading is fast
}

void StartMenu::calculateFPS()
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