#include "MainGame.h"
#include <Type3Engine/errors.h>
#include <iostream>
#include <string>
#include <Type3Engine/ResourceManager.h>


MainGame::MainGame() : 
	screenHeight_(768), 
	screenWidth_(1024), 
	time_(0.0f), 	   
	gameState_(GameState::PLAY),
	maxFPS_(60.0f)	   
{
	camera_.init(screenWidth_, screenHeight_);
}


MainGame::~MainGame()
{
}

void MainGame::run()
{

	initSystems();


	gameLoop();
}


void MainGame::initSystems()
{

	T3E::init();
	
	window_.create("Game Engine", screenWidth_, screenHeight_, T3E::BORDERLESS);
	initShaders();

	spriteBatch_.init();

	fpsLimiter_.init(maxFPS_);

}

void MainGame::initShaders()
{
	colourProgram_.compileShaders("Shaders/ColourShading.vert.txt", "Shaders/ColourShading.pix.txt");
	colourProgram_.addAttribute("vertexPosition");
	colourProgram_.addAttribute("vertexColour");
	colourProgram_.addAttribute("vertexUV");
	colourProgram_.linkShaders();
}	

void MainGame::gameLoop()
{
	//our game loop
	while (gameState_ != GameState::EXIT)
	{
		fpsLimiter_.begin();
		// used for frame time measuring
		float startTicks = SDL_GetTicks();

		camera_.update();

		processInput(); 
		time_ += 0.1f;
		renderGame();



		fps_ = fpsLimiter_.end();

				// print once every 10 frames
		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10)
		{
			std::cout << fps_ << std::endl;
			frameCounter = 0;
		}
		
	}

}

void MainGame::processInput()
{
	// processing our input
	SDL_Event evnt;

	const float CAMERA_SPEED = 2.0f;
	const float SCALE_SPEED = 0.1f;

	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			gameState_ = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			inputManager_.setMouseCoords(evnt.motion.x, evnt.motion.y);
			break;
		case SDL_KEYDOWN:
			inputManager_.keyPress(evnt.key.keysym.sym);
			break;
		case SDL_KEYUP:
			inputManager_.keyRelease(evnt.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			inputManager_.keyPress(evnt.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			inputManager_.keyRelease(evnt.button.button);
			break;
		
		default:
			break;
		}
	}

	if (inputManager_.isKeyPressed(SDLK_w))
	{
		camera_.setPosition(camera_.getPosition() + glm::vec2(0.0f, CAMERA_SPEED));
	}

	if (inputManager_.isKeyPressed(SDLK_s))
	{
		camera_.setPosition(camera_.getPosition() - glm::vec2(0.0f, CAMERA_SPEED));
	}

	if (inputManager_.isKeyPressed(SDLK_a))
	{
		camera_.setPosition(camera_.getPosition() - glm::vec2(CAMERA_SPEED, 0.0f));
	}

	if (inputManager_.isKeyPressed(SDLK_d))
	{
		camera_.setPosition(camera_.getPosition() + glm::vec2(CAMERA_SPEED, 0.0f));
	}
		
	if (inputManager_.isKeyPressed(SDLK_q))
	{
		camera_.setScale(camera_.getScale() + SCALE_SPEED);
	}
		
	if (inputManager_.isKeyPressed(SDLK_e))
	{
		camera_.setScale(camera_.getScale() - SCALE_SPEED);
	}

}

void MainGame::renderGame()
{


	//rendering our game
	glClearDepth(1.0);
	//clear both buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	colourProgram_.use();

	GLuint timeLocation = colourProgram_.getUniformLocation("time");
	glUniform1f(timeLocation, time_);

	glActiveTexture(GL_TEXTURE0);
	GLint textureLocation = colourProgram_.getUniformLocation("mySampler");
	glUniform1i(textureLocation, 0);

	//set the camera matrix
	GLuint pLocation = colourProgram_.getUniformLocation("P");
	glm::mat4 cameraMatrix = camera_.getCameraMatrix();

	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(cameraMatrix[0][0]));

	spriteBatch_.begin();

	glm::vec4 pos(0.0f, 0.0f, 50.0f, 50.0f);
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	static T3E::GLTexture texture = T3E::ResourceManager::getTexture("Textures/jimmyJump_pack/PNG/CharacterRight_Standing.png");
	T3E::Colour colour;
	colour.r = 255;
	colour.g = 255;
	colour.b = 255;
	colour.a = 255;

	spriteBatch_.draw(pos, uv, texture.id, 0.0f, colour);
	//spriteBatch_.draw(pos+ glm::vec4(50, 0, 0, 0), uv, texture.id, 0.0f, colour);


	spriteBatch_.end();

	spriteBatch_.renderBatch();



	glBindTexture(GL_TEXTURE_2D, 0);
	
	colourProgram_.stopUse();
	// swap our buffers 
	window_.swapBuffer();
}
