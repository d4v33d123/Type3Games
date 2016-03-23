#ifndef WINDOW_H
#define WINDOW_H

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif//__ANDROID__
#include "SDL.h"
#include <string>
#include "errors.h"

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

		int create( std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlag );
		void destroy();

		void swapBuffer();
		
		void updateSizeInfo(); // updatescreenWidth_ and screenHeight_ with the current window size
		int getScreenWidth() { return screenWidth_; }
		int getScreenHeight() { return screenHeight_; }
		
	private:
		SDL_Window* sdlWindow_;
		SDL_GLContext glContext_;
		int screenWidth_, screenHeight_;
	};
}

#endif