#ifndef BUTTON_H
#define BUTTON_H

#include "SDL.h"
#include "Type3Engine/AnimatedSprite.h"
#include <string>

namespace T3E
{
	class Button
	{
	public:
		//init sprite and record pos info
		//position and size in screen coordinates
		void init(int xPos, int yPos, int width, int height, std::string texturePath,
			float tileX, float tileY, float tileWidth, float tileHeight, int numFrames);
		
		//[in] touch position in screen coords
		//[ret] true if touch coords lie inside button sprite
		bool touchCollides(glm::vec2 touchPos);
		
		//getters	
		//yeh i be rturnin a pointah, whutchagunnadoo 'bout it, huh? yeeeeh
		AnimatedSprite* getSprite(){return &sprite_;};
		
	private:
		//TODO: animated sprite or normal sprite or...? wait to see if we're using multiple images or just highlight
		AnimatedSprite sprite_;
		bool isPressed;
	};
}

#endif