#include <SDL/SDL.h>
#include <GL/glew.h>
#include "Type3Engine.h"

namespace T3E
{

	int init()
	{
		// initialise SDL
		SDL_Init(SDL_INIT_EVERYTHING);

		// tells sdl we want to double buffer
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		
		return 0;
	}
}