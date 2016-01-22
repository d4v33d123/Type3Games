#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <string>

namespace T3E
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
		int getScreenWidth(){ screenWidth_; }
		int getScreenHeight(){ screenHeight_; }

		void swapBuffer();

	private:
		SDL_Window* sdlWindow_;
		int screenWidth_, screenHeight_;
	};

}