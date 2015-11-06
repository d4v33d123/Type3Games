#include "MainGame.h"
#include <Type3Engine/errors.h>
#include <iostream>
#include <string>


MainGame::MainGame() : 
	_screenHeight(768), 
	_screenWidth(1024),
	_time(0.0f), 
	_gameState(GameState::PLAY),
	_maxFPS(60.0f)
{
	
}


MainGame::~MainGame()
{
}

void MainGame::run()
{

	initSystems();
	_sprites.push_back(new Type3Engine::Sprite());
	_sprites.back()->init(-1.0f, -1.0f, 1.0f, 1.0f,"Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");
	_sprites.push_back(new Type3Engine::Sprite());
	_sprites.back()->init(0.0f, -1.0f, 1.0f, 1.0f, "Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");

	//_playerTexture = ImageLoader::loadPNG();

	gameLoop();
}


void MainGame::initSystems()
{

	Type3Engine::init();
	
	_window.create("Game Engine", _screenWidth, _screenHeight, Type3Engine::BORDERLESS);
	initShaders();

}

void MainGame::initShaders()
{
	_colourProgram.compileShaders("Shaders/ColourShading.vert.txt", "Shaders/ColourShading.pix.txt");
	_colourProgram.addAttribute("vertexPosition");
	_colourProgram.addAttribute("vertexColour");
	_colourProgram.addAttribute("vertexUV");
	_colourProgram.linkShaders();
}

void MainGame::gameLoop()
{
	//our game loop
	while (_gameState != GameState::EXIT)
	{
		// used for frame time measuring
		float startTicks = SDL_GetTicks();



		processInput(); 
		_time += 0.1f;
		renderGame();
		calculateFPS();

		// print once every 10 frames
		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10)
		{
			std::cout << _fps << std::endl;
			frameCounter = 0;
		}

		float frameTicks = SDL_GetTicks() - startTicks;
		//Limit the FPS to the max FPS
		if (1000.0f / _maxFPS > frameTicks)
		{
			SDL_Delay(1000.0f / _maxFPS - frameTicks);
		}
		
	}

}

void MainGame::processInput()
{
	// processing our input
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			//std::cout << evnt.motion.x << " " << evnt.motion.y << std::endl;
			break;
		default:
			break;
		}
	}

}

void MainGame::renderGame()
{


	//rendering our game
	glClearDepth(1.0);
	//clear both buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_colourProgram.use();

	GLuint timeLocation = _colourProgram.getUniformLocation("time");
	glUniform1f(timeLocation, _time);

	glActiveTexture(GL_TEXTURE0);
	GLint textureLocation = _colourProgram.getUniformLocation("mySampler");
	glUniform1i(textureLocation, 0);
	for (int i = 0; i < _sprites.size(); i++)
	{
		_sprites[i]->draw();
	}
	

	glBindTexture(GL_TEXTURE_2D, 0);
	
	_colourProgram.stopUse();
	// swap our buffers 
	_window.swapBuffer();
}

void MainGame::calculateFPS()
{
	static const int NUM_SAMPLES = 1000;
	static float frameTimes[NUM_SAMPLES];
	static int currentFrame = 0;


	static float prevTicks = SDL_GetTicks();

	float currentTicks;
	currentTicks = SDL_GetTicks();

	_frameTime = currentTicks - prevTicks;
	frameTimes[currentFrame % NUM_SAMPLES] = _frameTime;

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
		_fps = 1000.0 / frameTimeAverage;
	}
	else
	{
		_fps = 60.0f;
	}



}