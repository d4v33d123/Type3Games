#pragma once
#include <GL/glew.h>
#include "glTexture.h"
#include <string>
namespace T3E
{
	class Sprite
	{
	public:
		Sprite();
		~Sprite();

		void init(float x, float y, float width, float height, std::string texturePath);

		void draw();

	private:
		float x_, y_, width_, height_;
		GLuint vboID_;
		GLTexture texture_;
	};

}