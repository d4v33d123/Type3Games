#pragma once


#include <SDL/SDL.h>
#include <GL/glew.h>
#include <Type3Engine/Sprite.h>
#include <Type3Engine/GLSLProgram.h>
#include <Type3Engine/glTexture.h>
#include <Type3Engine/window.h>
#include <vector>
#include <Type3Engine/Type3Engine.h>
#include <glm/glm.hpp>
#include <Type3Engine/Camera2D.h>
#include <Type3Engine/SpriteBatch.h>
#include <Type3Engine/InputManager.h>
#include <Type3Engine/Timing.h>



enum class GameState {PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();
	



private:
	
	void initSystems();
	void initShaders();
	void gameLoop();
	void processInput();
	void renderGame();
	void calculateFPS();


	T3E::window window_;
	int screenWidth_;
	int screenHeight_;
	GameState gameState_;

	T3E::Camera2D camera_;
	T3E::SpriteBatch spriteBatch_;
	T3E::GLSLProgram colourProgram_;
	T3E::InputManager inputManager_;
	T3E::FpsLimiter fpsLimiter_;
	
	float maxFPS_;
	float time_;
	float fps_;
};

