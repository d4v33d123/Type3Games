#include "Type3Engine/Type3Engine.h"
#include <time.h>
#include "command.h"
#include "StartMenu.h"
#include "Tutorial.h"
#include "MainGame.h"


int main(int argc, char** argv)
{
	T3E::window window;
	StartMenu* startMenu;
	Tutorial* tutorial;
	MainGame* mainGame;
	
	//setup game, window, render flags
	srand(time(NULL));
	T3E::init();
	window.create("Type3Games - Mutagenesis", 800, 600, T3E::BORDERLESS);//screen size is overwritten on android devices to be full screen
	//enable back face culling
	glEnable(GL_CULL_FACE);//GL_BACK is default value
	//TODO: enable blend once vs enable/disable when needed?
	glEnable( GL_BLEND );//should we instead use frame buffer fetch in shader?
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	//set line width for grid
	glLineWidth(5.0f);
	
	//play
	
	//splashscreen here?
	
	command c = command::MENU;
	//start menu
	while(c != command::QUIT)
	{
		switch(c)
		{
		case command::PLAY:
			mainGame = new MainGame();
			c = mainGame->run(&window);
			delete mainGame;
			mainGame = NULL;
			break;
		case command::TUTORIAL:
			tutorial = new Tutorial();
			c = tutorial->run(&window);
			delete tutorial;
			tutorial = NULL;
			break;
		case command::MENU:
			startMenu = new StartMenu();
			c = startMenu->run(&window);
			delete startMenu;
			startMenu = NULL;
			break;
		default:
			break;
		}
	}	
	//end screen here?

	//cleanup
	T3E::ResourceManager::clearTextures();
	SDL_Quit();	
	return 0;
}