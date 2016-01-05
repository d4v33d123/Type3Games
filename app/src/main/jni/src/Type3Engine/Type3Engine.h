#ifndef TYPE3ENGINE_H
#define TYPE3ENGINE_H

#include "SDL.h"

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif//__ANDROID__
	
namespace Type3Engine
{
	extern int init();
}

#endif