#ifndef ANIMATED_SPRITE_H
#define ANIMATED_SPRITE_H

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
	class AnimatedSprite
	{
	public:
		AnimatedSprite();
		~AnimatedSprite();

		//UV COORDS ARE SWAPPED!!! invert width with height and x with y for tiling, position in spritesheet etc
		void init(float x, float y, float width, float height, std::string texturePath, float tileX, float tileY, float tileWidth, float tileHeight, int numSprites);

		void draw();
		void Update(float deltaTime);

		void setSpeed(float s){animSpeed_ = s;};

		//getters
		glm::vec2 getPos(){return glm::vec2(x_, y_);};
		float getWidth(){return width_;};
		float getHeight(){return height_;};
		
	private:
		int animPosition_;
		float x_;
		float y_;
		float counter;
		float animSpeed_;
		int numSprites_;
		float animCount_;
		float animEnd_;
		float width_, height_, tileWidth_, tileHeight_, tileX_, tileY_;		
		GLuint vboID_;
		GLTexture texture_;
		
	};

}

#endif

