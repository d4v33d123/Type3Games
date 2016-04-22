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

        /**
		*
		* Create a window
        * Note: on android the dimensions will be overwritten to fit the full screen size
		*
		* @param [in] name of the window (e.g. the one displayed in the toolbar on pc)
		* @param [in] width of the window in pixels
        * @param [in] height of the window in pixels
		* @param [in] SDL flags for window options (e.g BORDERLESES, FULLSCREEN, ...)
        *
		*/
		int create( std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlag );
        
        /**
		*
		* Free the context and  the SDL_window member
        *
		*/
		void destroy();

        /**
		*
		* Swap the current buffer with the other one to display its content on screen (double buffering)
        *
		*/
		void swapBuffer();
		
        /**
		*
		* load the actual current window dimensions in the screenWidth_ and screenHeight_ members
        * -use this at least once before using getters for the dimensions of the window
        * -use this again if the window size changes (e.g. on a phone, landscape to normal layout switch)
        *
		*/
		void updateSizeInfo(); // updatescreenWidth_ and screenHeight_ with the current window size
        
        //@{
        /** Getters for window size*/
		int getScreenWidth() { return screenWidth_; }
		int getScreenHeight() { return screenHeight_; }
        //@}
		
	private:
		SDL_Window* sdlWindow_;
		SDL_GLContext glContext_;
		int screenWidth_, screenHeight_;
	};
}

#endif