#pragma once
#include <GL/glew.h>
#include "glTexture.h"
#include <string>
namespace Type3Engine
{
	class Sprite
	{
	public:
		Sprite();
		~Sprite();

		void init(float x, float y, float width, float height, std::string texturePath);

		void draw();

	private:
		float _x, _y, _width, _height;
		GLuint _vboID;
		GLTexture _texture;
	};

}