#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif//__ANDROID__
	
namespace T3E
{
	struct GLTexture
	{
		GLuint id;
		int width;
		int height;
		GLuint unit;
		static GLuint numTextures;
	};
}

#endif