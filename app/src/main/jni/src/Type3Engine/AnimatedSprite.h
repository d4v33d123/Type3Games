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
		
		/**
		* Initialise the Animated sprite
		*
		* @param [in] x position in the world
		* @param [in] y position in the world
		* @param [in] width of the sprite
		* @param [in] height of the sprite
		* @param [in] texture file path
		* @param [in] UV position x on the spritesheet
		* @param [in] UV position y on the spritesheet
 		* @param [in] width of a sprite on the spritesheet
		* @param [in] height of a sprite on the spritesheet
		* @param [in] the number of sprites in the animation
		* @param [in] the number of frames in each line of the sprite sheet
		*/
		//UV COORDS ARE SWAPPED!!! invert width with height and x with y for tiling, position in spritesheet etc
		void init(float x, float y, float width, float height, std::string texturePath,
			float tileX, float tileY, float tileWidth, float tileHeight, int numSprites, int framesPerLine);

		/**
		* Draw the sprite
		*/
		void draw();
		
		/**
		* Update the animated sprite
		* 
		* @param [in] delta time
		* @return [out] false if the sprite fails to update, true if it succeeds
		*/
		//return true when reached end frame of animation
		bool Update(float deltaTime);

		/**
		* Sets the speed of the animation
		*
		* @param [in] a multiplier of deltatime to control the speed of the animation
		*/
		void setSpeed(float s){animSpeed_ = s;};
		
		void refresh(){needRefresh_ = true;};
		
	private:
		
		void updateUVs();//refresh uv coordinates and rebind geometry
		
		int animPosition_; // The position of the animation out of the full animation
		float animSpeed_; // multiplier of delta time to control the speed of the animation
		int numSprites_; // the number of sprites in the animation
		float animCount_; // the time since the last frame change
		float animEnd_; // the time between each fram change
		int framesPerLine_; // the number of frames in each line of the spritesheet
		float originalTileX_;
		bool needRefresh_;//after finishing a loop, when restarting we need to reset uv coords in update()

	};

}

#endif

