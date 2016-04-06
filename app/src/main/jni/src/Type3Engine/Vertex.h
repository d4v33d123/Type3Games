#ifndef VERTEX_H
#define VERTEX_H

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif//__ANDROID__
	
namespace T3E
{
	struct Position
	{
		GLfloat x;
		GLfloat y;
	};

	struct UV
	{
		GLfloat v;
		GLfloat u;
	};

	struct Vertex
	{
		Position position;

		UV uv;

		void setPosition(float x, float y)
		{
			position.x = x;
			position.y = y;
		}
		void setUV(float u, float v)
		{
			uv.v = v;
			uv.u = u;
		}
	};
}

#endif