#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "glTexture.h"
#include <string>
#include "TileSheet.h"

namespace T3E
{
	class Sprite
	{
	public:
		Sprite();
		~Sprite();

		void init(float x, float y, float width, float height, std::string texturePath, float TileWidth, float TileHeight, int fps);

		void draw();
		void Update(float deltaTime);

	private:
		int animationPosition_;
		float counter;
		int fps_;
		float x_, y_, width_, height_;
		GLuint vboID_;
		TileSheet tileSheet_;
		GLTexture texture_;
	};

}