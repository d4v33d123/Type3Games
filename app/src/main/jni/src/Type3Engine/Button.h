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
		void init(float posX, float posY, float width, float height,
			std::string texturePath, float tileWidth, float tileHeight,
			float unpressedX, float unpressedY,
			float pressedX, float pressedY);
			
		//[in] touch position in screen coords
		//[ret] true if touch coords lie inside button sprite
		// test is made on unpressedSprite_
		bool touchCollides(glm::vec2 touchPos);
		
		//draw correct sprite depending if it's pressed or unpressed
		void draw();
		
		//Setters
		void press(){isPressed_ = true;};
		void unpress(){isPressed_ = false;};
		void toggle(){isPressed_ = !isPressed_;};
		
		//getters
		bool isPressed(){return isPressed_;};
		
		GLuint getTexUnit() { return pressedSprite_.getTexUnit(); }

	private:
		Sprite unpressedSprite_, pressedSprite_;
		bool isPressed_;
	};
}

#endif