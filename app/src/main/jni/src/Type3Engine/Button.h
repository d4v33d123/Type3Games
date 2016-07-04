#ifndef BUTTON_H
#define BUTTON_H

#include "SDL.h"
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
		* test is made on the unpressed sprite
		*
		* @param [in] touch position on the screen in pixels
		* @return [out] return true if the coordinates lie inside the sprite, false if they do not
		*/
		bool touchCollides( SDL_Point touchPos );
		
		/**
		* draws the pressed or unpressed sprite depending on it's condition 
		*/
		void draw();		
		
		//Setters
		/**
		* sets the button as being pressed
		*/
		void press() { isPressed_ = true; }
		
		/**
		* sets the button as not being pressed
		*/
		void unpress() { isPressed_ = false; }
		
		/**
		* toggles whether or not the button has been pressed or not
		*/
		void toggle() { isPressed_ = !isPressed_; }
		
		//getters
		/**
		* checks to see if the button is pressed or not 
		*
		* @return [out] the true or false value of if the button has been pressed or not
		*/
		bool isPressed() { return isPressed_; }
		
		/**
		* Gets the texture unit of the pressed sprite 
		*
		* @return the GLuint containing the texture unit of the pressed sprite
		*/
		GLuint getTexUnit() { return pressedSprite_.getTexUnit(); }

	private:
		Sprite unpressedSprite_, pressedSprite_;
		bool isPressed_;
	};
}

#endif