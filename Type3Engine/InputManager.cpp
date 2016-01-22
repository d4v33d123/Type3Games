#include "InputManager.h"

namespace T3E
{
	InputManager::InputManager()
	{
	}


	InputManager::~InputManager()
	{
	}

	void InputManager::keyPress(unsigned int keyID)
	{
		keyMap_[keyID] = true;
	}

	void InputManager::keyRelease(unsigned int keyID)
	{
		keyMap_[keyID] = false;
	}

	void InputManager::setMouseCoords(float x, float y)
	{
		mouseCoords.x = x;
		mouseCoords.y = y;
	}

	bool InputManager::isKeyPressed(unsigned int keyID)
	{
		auto it = keyMap_.find(keyID);
		if (it != keyMap_.end())
		{
			return it->second;
		}
		else
		{
			return false;
		}
	}

	

}

