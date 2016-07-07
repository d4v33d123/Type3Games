#include "Type3Engine/Type3Engine.h"
#include "Type3Engine/AudioEngine.h"
#include "Type3Engine/TextRenderer.h"
#include "Type3Engine/window.h"
#include <time.h>
#include "command.h"
#include "StartMenu.h"
#include "Credits.h"
#include "MainGame.h"
#include "Type3Engine/glTexture.h"

/** \mainpage
*
* \section welcome Cell Cycle
*
* Welcome to the documentation for _Cell Cycle_!
*
* _Cell Cycle_ is a cellular simulation game for Android.
*
* The documentatin can be navigated using either the tabs at the top, or the tree view to the side.
*
* The project is written in C++ and uses [SDL2](http://libsdl.org), [SDL2_mixer](http://www.libsdl.org/projects/SDL_mixer/), OpenGLES 2.0 and GLM.
*
* Programmers:
* * David Robertson
* * Davide Passaniti
* * Thomas Hope
*
*/

int main(int argc, char** argv)
{
	srand(time(NULL));
	T3E::window window;
	T3E::AudioEngine audioEngine;
	T3E::TextRenderer textRenderer;
	T3E::Music menuMusic;
	
	// Scenes
	StartMenu* startMenu;
	Credits* credits;
	MainGame* mainGame;
	
	// Setup game, window, render flags
	T3E::init();
	window.create("Type3Games - CellCycle", 800, 600, T3E::BORDERLESS);//screen size is overwritten on android devices to be full screen
	
	// Setup the audio engine
	audioEngine.init();
	menuMusic = audioEngine.loadMusic("sound/menu_music_v2.ogg");
	menuMusic.play(-1);

	// Setup the text renderer
	textRenderer.init();
	textRenderer.setScreenSize( window.getScreenWidth(), window.getScreenHeight() );

    // Enable aplha blending	
	glEnable( GL_BLEND );		// TODO: Should we instead use frame buffer fetch in shader?
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	// Initalise the game to the start menu
	command c = command::MENU;

	/**
	* This is the main application state controll
	* Each state has a run function which is called and returns a command.
	* The next state is chosen based on the returned command
	*/
	while(c != command::QUIT)
	{
		switch(c)
		{
		case command::PLAY:
			mainGame = new MainGame();
			c = mainGame->run(&window, &audioEngine, &textRenderer, false);
			delete mainGame;
			mainGame = NULL;
			menuMusic.play(-1);//put menu music back on
			break;
		case command::TUTORIAL:
			mainGame = new MainGame();
			c = mainGame->run(&window, &audioEngine, &textRenderer, true);
			delete mainGame;
			mainGame = NULL;
			break;
		case command::CREDITS:
			credits = new Credits();
			c = credits->run(&window, &audioEngine, &textRenderer);
			delete credits;
			credits = NULL;
			break;
		case command::MENU:
			startMenu = new StartMenu();
			c = startMenu->run(&window, &audioEngine, &textRenderer);
			delete startMenu;
			startMenu = NULL;
			break; 
		default:
			break;
		}
	}

	// Application Cleanup	
	textRenderer.destroy();
	T3E::ResourceManager::clearTextures();
	T3E::GLTexture::numTextures = 0;
	SDL_Quit();

	exit(0); // Temp fix to music bug...
	return 0;
}