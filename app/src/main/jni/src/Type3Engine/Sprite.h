#ifndef SPRITE_H
#define SPRITE_H

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif//__ANDROID__
#include "Vertex.h"
#include <cstddef>
#include "ResourceManager.h"
#include "glTexture.h"
#include <string>
#include "glm/glm.hpp"

namespace T3E
{
	class Sprite
	{
	public:
		Sprite();
		~Sprite();
		
		//UV COORDS ARE SWAPPED!!! invert width with height and x with y for tiling, position in spritesheet etc
		void init(float x, float y, float width, float height, std::string texturePath,float tileX, float tileY, float tileWidth, float tileHeight);

		void draw();
		
		//getters
		glm::vec2 getPos()	{ return glm::vec2(x_, y_); }
		float getWidth()	{ return width_; }
		float getHeight()	{ return height_; }
		GLuint getVbo()		{ return vboID_; }
		GLTexture getTexture() { return texture_; }
		GLuint getTexUnit() { return texture_.unit; }
		
	protected:
		float x_, y_, width_, height_, tileWidth_, tileHeight_, tileX_, tileY_;		
		GLuint vboID_;
		GLTexture texture_;

		static GLuint boundTextureUnit_;
	};
}

#endif