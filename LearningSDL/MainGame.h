#pragma once


#include <SDL/SDL.h>
#include <GL/glew.h>
#include <Type3Engine/Sprite.h>
#include <Type3Engine/GLSLProgram.h>
#include <Type3Engine/glTexture.h>
#include <Type3Engine/window.h>
#include <vector>
#include <Type3Engine/Type3Engine.h>


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


	Type3Engine::window _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;

	std::vector<Type3Engine::Sprite*> _sprites;


	
	Type3Engine::GLSLProgram _colourProgram;
	
	float _time;
	float _fps;
	float _frameTime;
	float _maxFPS;
};

