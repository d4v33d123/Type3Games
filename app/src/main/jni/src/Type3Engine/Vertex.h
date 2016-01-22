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
		float x;
		float y;
	};

	struct Colour
	{
		GLubyte r;
		GLubyte g;
		GLubyte b;
		GLubyte a;
	};

	struct UV
	{
		float v;
		float u;
	};

	struct Vertex
	{
		// position
		Position position;
		//colour
		Colour colour;
		//texturecoordinates
		UV uv;

		void setPosition(float x, float y)
		{
			position.x = x;
			position.y = y;
		}

		void setColour(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
		{
			colour.r = r;
			colour.g = g;
			colour.b = b;
			colour.a = a;
		}

		void setUV(float u, float v)
		{
			uv.v = v;
			uv.u = u;
		}
	};
}

#endif