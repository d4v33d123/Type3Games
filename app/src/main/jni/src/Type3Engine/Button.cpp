#include "Button.h"

namespace T3E
{
	void Button::init(float posX, float posY, float width, float height,
			std::string texturePath, float tileWidth, float tileHeight,
			float unpressedX, float unpressedY,
			float pressedX, float pressedY)
	{
		isPressed_ = false;
		unpressedSprite_.init(posX, posY, width, height, texturePath, unpressedX, unpressedY, tileWidth, tileHeight);
		pressedSprite_.init(posX, posY, width, height, texturePath, pressedX, pressedY, tileWidth, tileHeight);
	}
	
	bool Button::touchCollides(glm::vec2 touchPos)
	{				
		float left = unpressedSprite_.getPos().x;
		float right = unpressedSprite_.getPos().x + unpressedSprite_.getWidth();
		float top = unpressedSprite_.getPos().y;
		float bottom = unpressedSprite_.getPos().y + unpressedSprite_.getHeight();
		
		if(touchPos.x < left) return false;
		if(touchPos.x > right) return false;
		if(touchPos.y > bottom) return false;
		if(touchPos.y < top) return false;
		
		return true;
	}
	
	void Button::draw()
	{
		if(isPressed_)
			pressedSprite_.draw();
		else
			unpressedSprite_.draw();
	}
}