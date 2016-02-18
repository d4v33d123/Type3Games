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

namespace T3E
{
	class AnimatedSprite
	{
	public:
		AnimatedSprite();
		~AnimatedSprite();

		void init(float x, float y, float width, float height, std::string texturePath, float TileWidth, float TileHeight, int numSprites);

		void draw();
		void Update(float deltaTime);

		void setSpeed(float s){animSpeed_ = s;};
		
	private:
		int animPosition_;
		float x_;
		float y_;
		float counter;
		float animSpeed_;
		int numSprites_;
		float animCount_;
		float animEnd_;
		float width_, height_, tileWidth_, tileHeight_;		
		GLuint vboID_;
		GLTexture texture_;
		
	};

}

#endif

