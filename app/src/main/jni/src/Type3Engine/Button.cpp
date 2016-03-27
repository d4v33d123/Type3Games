#include "Button.h"

namespace T3E
{
	void Button::init(int xPos, int yPos, int width, int height, std::string texturePath,
		float tileX, float tileY, float tileWidth, float tileHeight, int numFrames)
	{
		sprite_.init(xPos, yPos, width, height, texturePath);//, tileX, tileY, tileWidth, tileHeight, numFrames);
		
	}
	
	bool Button::touchCollides(glm::vec2 touchPos)
	{				
		float left = sprite_.getPos().x;
		float right = sprite_.getPos().x + sprite_.getWidth();
		float top = sprite_.getPos().y;
		float bottom = sprite_.getPos().y + sprite_.getHeight();
		
		if(touchPos.x < left) return false;
		if(touchPos.x > right) return false;
		if(touchPos.y > bottom) return false;
		if(touchPos.y < top) return false;
		
		return true;
	}
}