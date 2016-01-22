#include "window.h"

namespace T3E
{
	window::window()
	{
	}

	window::~window()
	{
	}

	int window::create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlag)
	{
		//SDL_GL_LoadLibrary(NULL);done automatically upon creation of first wondow, also unload?
		Uint32 flags = SDL_WINDOW_OPENGL;

		if (currentFlag & INVISIBLE)
		{
			flags |= SDL_WINDOW_HIDDEN;
		}
		if (currentFlag & FULLSCREEN)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		if (currentFlag & BORDERLESS)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		// setting up our window
		sdlWindow_ = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
		if (sdlWindow_ == nullptr)
		{
			fatalError("SDL WINDOW COULD NOT BE CREATED");
		}

		// setting up open gl context
		SDL_GLContext glContext = SDL_GL_CreateContext(sdlWindow_);
		if (glContext == nullptr)
		{
			fatalError("SDL_GL CONTEXT COULD NOT BE CREATED");
		}

		// check the version of open gl that is being used
		SDL_Log("*** OpenGL version %s ***\n", glGetString(GL_VERSION));

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// set to 1 to turn on vsync
		SDL_GL_SetSwapInterval(1);

		return 0;
	}

	void window::swapBuffer()
	{
		SDL_GL_SwapWindow(sdlWindow_);
	}
	
	void window::destroy()
	{
		SDL_DestroyWindow(sdlWindow_);
	}
	
	void window::updateSizeInfo()
	{
		SDL_GetWindowSize(sdlWindow_, &screenWidth_, &screenHeight_);
	}
	
}