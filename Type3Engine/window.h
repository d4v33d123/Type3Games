#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <string>

namespace Type3Engine
{

	enum windowFlag
	{
		INVISIBLE = 0x1,
		FULLSCREEN = 0x2,
		BORDERLESS = 0x4
	};

	class window
	{
	public:
		window();
		~window();

		int create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlag);
		int getScreenWidth(){ _screenWidth; }
		int getScreenHeight(){ _screenHeight; }

		void swapBuffer();

	private:
		SDL_Window* _sdlWindow;
		int _screenWidth, _screenHeight;
	};

}