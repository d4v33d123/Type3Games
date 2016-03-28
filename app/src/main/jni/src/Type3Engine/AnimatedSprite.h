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
#include "Sprite.h"

namespace T3E
{
	class AnimatedSprite: public Sprite
	{
	public:
		AnimatedSprite();
		~AnimatedSprite();

		//UV COORDS ARE SWAPPED!!! invert width with height and x with y for tiling, position in spritesheet etc
		void init(float x, float y, float width, float height, std::string texturePath,
			float tileX, float tileY, float tileWidth, float tileHeight, int numSprites, int framesPerLine);

		void draw();
		
		//return true when reached end frame of animation
		bool Update(float deltaTime);

		void setSpeed(float s){animSpeed_ = s;};
		
	private:
		int animPosition_;
		float counter;
		float animSpeed_;
		int numSprites_;
		float animCount_;
		float animEnd_;
		int framesPerLine_;
	};

}

#endif

