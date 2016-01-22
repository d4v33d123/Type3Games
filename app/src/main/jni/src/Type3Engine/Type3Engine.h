#ifndef T3E_H
#define T3E_H

#include "SDL.h"

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif//__ANDROID__
	
namespace T3E
{
	extern int init();
}

#endif