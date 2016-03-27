#include "StartMenu.h"

command StartMenu::run(T3E::window* window)
{SDL_Log("in s menu");
	window_ = window;
	initSystems();SDL_Log("initted sys in s menu");
	backgroundMusic_ = audioEngine_.loadMusic("sound/menu_music_v2.ogg");
	backgroundMusic_.play(-1);
	
	buttonPress_ = audioEngine_.loadSoundEffect("sound/Player_CellDivide_Move.ogg");

	command result = gameLoop();

	//backgroundMusic_.stop();

	return result;
}

void StartMenu::initSystems()
{	SDL_Log("initting sys in s menu");
	audioEngine_.init();SDL_Log("done");
	//init ortho matrix
	//inverting top with bottom to avoid sprites being drawn upside down
	//note that this will put origin at bottom left, while screen coords have origin at top left
	window_->updateSizeInfo();
	orthoM_ = glm::ortho(0.0f, float( window_->getScreenWidth() ), 0.0f, float( window_->getScreenHeight() ));
SDL_Log("audio and projection ok");
    // Set the first cell
    //grid_.newCell( 21, 23, T3E::CellState::STEM, 0, nullptr );
	//Create menu buttons and bg sprite
	playButton_.init(float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())*(5.0f/7.0f),
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/7.0f, "textures/bvbutton.png", 0, 0, 1.0f/2, 1.0f/2, 2);	
	SDL_Log("play b ok");
	tutorialButton_.init(float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())*(3.0f/7.0f),
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/7.0f, "textures/bvbutton.png", 0, 0, 1.0f/2, 1.0f/2, 2);
	quitButton_.init(float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())*(1.0f/7.0f),
		float(window_->getScreenWidth())/3.0f, float(window_->getScreenHeight())/7.0f, "textures/bvbutton.png", 0, 0, 1.0f/2, 1.0f/2, 2);
SDL_Log("all b ok");
	backgroundSprite_.init(0.0f, 0.0f, float(window_->getScreenWidth()), float(window_->getScreenHeight()),"textures/background.png");
	SDL_Log("bg ok");
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
	tintedSpriteProgram_.addAttribute("aColour");
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
	while((c = processInput()) == command::NONE)
	{
		renderGame();
	}
	
	return c;
}

command StartMenu::processInput()
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
			
		case SDL_FINGERUP:
			//get touch pos in screen coordinates for UI interaction
			//invert y to match our ortho projection (origin at bottom left for ease of life)
			screenCoords = glm::vec2(evnt.tfinger.x * float(window_->getScreenWidth()),
				float(window_->getScreenHeight()) - evnt.tfinger.y * float(window_->getScreenHeight()));
				
			if(playButton_.touchCollides(screenCoords))
			{
				return command::PLAY;	
			}
			if(tutorialButton_.touchCollides(screenCoords))
			{
				return command::TUTORIAL;	
			}
			if(quitButton_.touchCollides(screenCoords))
			{
				return command::QUIT;
			}
			return command::NONE;
			break;
			
		default:
			return command::NONE;
		}
	}	
}

void StartMenu::renderGame()
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
	GLint texid = T3E::ResourceManager::getTexture("textures/background.png").id;
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
	texid = T3E::ResourceManager::getTexture("textures/bvbutton.png").id;
	glActiveTexture(GL_TEXTURE0 + texid);	
	glUniform1i(sampler0_location, texid);
	//draw sprite
	playButton_.getSprite()->draw();
	tutorialButton_.getSprite()->draw();
	quitButton_	.getSprite()->draw();
	
	tintedSpriteProgram_.stopUse();	

	// swap our buffers 
	window_->swapBuffer();
}