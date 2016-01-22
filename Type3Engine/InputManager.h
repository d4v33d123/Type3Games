#pragma once

#include <unordered_map>
#include <glm/glm.hpp>

namespace T3E
{
	class InputManager
	{
	public:
		InputManager();
		~InputManager();

		void keyPress(unsigned int keyID);
		void keyRelease(unsigned int keyID);

		void setMouseCoords(float x, float y);

		bool isKeyPressed(unsigned int keyID);
	private:
		std::unordered_map<unsigned int, bool> keyMap_;
		glm::vec2 mouseCoords;
	};
}


