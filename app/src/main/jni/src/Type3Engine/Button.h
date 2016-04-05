#ifndef BUTTON_H
#define BUTTON_H

#include "SDL.h"
//#include "Type3Engine/AnimatedSprite.h"
#include "Type3Engine/Sprite.h"
#include <string>

namespace T3E
{
	class Button
	{
	public:
		
		/**
		* Initialise the button
		*
		* @param [in] the x position of the button on the screen
		* @param [in] the y position of the button on the screen
		* @param [in] the width of the button sprite
		* @param [in] the height of the button sprite
		* @param [in] texture file path
 		* @param [in] width of a sprite on the spritesheet
		* @param [in] height of a sprite on the spritesheet
		* @param [in] the UV x position of the unpressed sprite on the spritesheet
		* @param [in] the UV y position of the unpressed sprite on the spritesheet
		* @param [in] the UV x position of the pressed sprite on the spritesheet
		* @param [in] the UV y position of the pressed sprite on the spritesheet
		*/
		void init(float posX, float posY, float width, float height,
			std::string texturePath, float tileWidth, float tileHeight,
			float unpressedX, float unpressedY,
			float pressedX, float pressedY);
			
		/**
		* checks to see if the button has been pressed by the player
		*
		* @param [in] touch position in screen coordinates
		* @return [out] return true if the coordinates lie inside the sprite, false if they do not
		*/
		//[in] touch position in screen coords
		//[ret] true if touch coords lie inside button sprite
		// test is made on unpressedSprite_
		bool touchCollides(glm::vec2 touchPos);
		
		/**
		* draws the sprite
		*/
		//draw correct sprite depending if it's pressed or unpressed
		void draw();
		
		
		//Setters
		
		void press(){isPressed_ = true;};
		void unpress(){isPressed_ = false;};
		void toggle(){isPressed_ = !isPressed_;};
		
		//getters
		bool isPressed(){return isPressed_;};
		//yeh i be rturnin a pointah, whutchagunnadoo 'bout it, huh? yeeeeh
		Sprite* getUnpressedSprite(){return &unpressedSprite_;};
		Sprite* getPressedSprite(){return &pressedSprite_;};
		
	private:
		Sprite unpressedSprite_, pressedSprite_;
		bool isPressed_;
	};
}

#endif