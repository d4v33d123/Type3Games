#include "Type3Engine/Type3Engine.h"
#include "Type3Engine/AudioEngine.h"
#include <time.h>
#include "command.h"
#include "StartMenu.h"
#include "Tutorial.h"
#include "Credits.h"
#include "MainGame.h"


int main(int argc, char** argv)
{
	T3E::window window;
	T3E::AudioEngine audioEngine;
	T3E::Music menuMusic;
	
	//scenes
	StartMenu* startMenu;
	Tutorial* tutorial;
	Credits* credits;
	MainGame* mainGame;
	
	//setup game, window, render flags
	srand(time(NULL));
	T3E::init();
	window.create("Type3Games - Mutagenesis", 800, 600, T3E::BORDERLESS);//screen size is overwritten on android devices to be full screen
	audioEngine.init();
	menuMusic = audioEngine.loadMusic("sound/menu_music_v2.ogg");
	menuMusic.play(-1);
	
	//play
	command c = command::MENU;
	//start menu
	while(c != command::QUIT)
	{
		switch(c)
		{
		case command::PLAY:
			mainGame = new MainGame();
			c = mainGame->run(&window, &audioEngine);
			delete mainGame;
			mainGame = NULL;
			menuMusic.play(-1);//put menu music back on
			break;
		case command::TUTORIAL:
			tutorial = new Tutorial();
			c = tutorial->run(&window, &audioEngine);
			delete tutorial;
			tutorial = NULL;
			break;
		case command::CREDITS:
			credits = new Credits();
			c = credits->run(&window, &audioEngine);
			delete credits;
			credits = NULL;
			break;
		case command::MENU:
			startMenu = new StartMenu();
			c = startMenu->run(&window, &audioEngine);
			delete startMenu;
			startMenu = NULL;
			break; 
		default:
			break;
		}
	}

	//cleanup
	audioEngine.clearMaps();//TESTING... does nothing
	T3E::ResourceManager::clearTextures();
	SDL_Quit();	
	return 0;
}