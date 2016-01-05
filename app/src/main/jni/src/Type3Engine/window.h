#ifndef WINDOW_H
#define WINDOW_H

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif//__ANDROID__
#include "SDL.h"
#include <string>
#include "errors.h"

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
		int getScreenWidth(){ return _screenWidth; }
		int getScreenHeight(){ return _screenHeight; }

		void swapBuffer();
		
		void destroy();//destroy _sdlWindow
		void updateSizeInfo();//update_screenWidth and _screenHeight with the current window size
		
	private:
		SDL_Window* _sdlWindow;
		int _screenWidth, _screenHeight;
	};
}

#endif