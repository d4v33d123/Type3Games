#include "window.h"
#include "errors.h"

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
		screenHeight_ = screenHeight;
		screenWidth_ = screenWidth;


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

		// setting up glew
		GLenum error = glewInit();
		if (error != GLEW_OK)
		{
			fatalError("COULD NOT INITIALISE GLEW");
		}

		// check the version of open gl that is being used
		std::printf("*** OpenGL version %s ***\n", glGetString(GL_VERSION));


		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

		// set to 1 to turn on vsync
		SDL_GL_SetSwapInterval(1);

		//Enable alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return 0;
	}

	void window::swapBuffer()
	{
		SDL_GL_SwapWindow(sdlWindow_);
	}
}