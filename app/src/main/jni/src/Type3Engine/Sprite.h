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
        /**
		*
		* Initialise a sprite object as a textured quad (2 triangles)
		*
		* @param [in] x position of the bottom left corner of the sprite in world coordinates
		* @param [in] y position of the bottom left corner of the sprite in world coordinates
        * @param [in] width of the sprite in world coordinates
		* @param [in] height of the sprite in world coordinates
        * @param [in] The string containing the file path to the png texture file        
        * @param [in] V coordinate in the spritesheet of the topleft corner of the sprite
        * @param [in] U coordinate in the spritesheet of the topleft corner of the sprite
        * @param [in] width of the sprite in UV coordinates
        * @param [in] height of the sprite in UV coordinates
        *
		*/
		void init(float x, float y, float width, float height, std::string texturePath,float tileX, float tileY, float tileWidth, float tileHeight);

        /**
		*
		* Draws the sprite with the shader currently in use
		*
		*/
		void draw();
		
		//@{
        /** Getters */
		glm::vec2 getPos()	{ return glm::vec2(x_, y_); }
		float getWidth()	{ return width_; }
		float getHeight()	{ return height_; }
		GLuint getVbo()		{ return vboID_; }
		GLTexture getTexture() { return texture_; }
		GLuint getTexUnit() { return texture_.unit; }
        //@}
		
		static void resetBoundTextureUnit() { boundTextureUnit_ = -1; }

	protected:
		float x_, y_, width_, height_, tileWidth_, tileHeight_, tileX_, tileY_;		
		GLuint vboID_;
		GLTexture texture_;

		static GLuint boundTextureUnit_;
	};
}

#endif