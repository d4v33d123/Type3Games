#include "Sprite.h"
#include "Vertex.h"
#include <cstddef>
#include "ResourceManager.h"

namespace T3E
{

	Sprite::Sprite()
	{
		vboID_ = 0;
	}


	Sprite::~Sprite()
	{
		if (vboID_ != 0)
		{
			glDeleteBuffers(1, &vboID_);
		}
	}

	void Sprite::init(float x, float y, float width, float height, std::string texturePath)
	{
		x_ = x;
		y_ = y;
		width_ = width;
		height_ = height;

		texture_ = ResourceManager::getTexture(texturePath);


		if (vboID_ == 0)
		{
			glGenBuffers(1, &vboID_);

		}

		Vertex vertexData[6];
		//top right

		vertexData[0].setPosition(x + width, y + height);
		vertexData[0].setUV(1.0f, 1.0f);
		//top left

		vertexData[1].setPosition(x, y + height);
		vertexData[1].setUV(0.0f, 1.0f);
		//bottom left

		vertexData[2].setPosition(x, y);
		vertexData[2].setUV(0.0f, 0.0f);

		//bottom left
		vertexData[3].setPosition(x, y);
		vertexData[3].setUV(0.0f, 0.0f);
		//bottom right
		vertexData[4].setPosition(x + width, y);
		vertexData[4].setUV(1.0f, 0.0f);
		//top right
		vertexData[5].setPosition(x + width, y + height);
		vertexData[5].setUV(1.0f, 1.0f);

		for (int i = 0; i < 6; i++)
		{
			vertexData[i].setColour(255, 0, 255, 255);
		}

		vertexData[1].setColour(0, 0, 255, 255);


		vertexData[4].setColour(0, 255, 0, 255);


		glBindBuffer(GL_ARRAY_BUFFER, vboID_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	void Sprite::draw()
	{

		// bind the texture
		glBindTexture(GL_TEXTURE_2D, texture_.id);// dont want to unbind this

	

		// draw our 6 verticies
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// disable the vertex attrib array
		glDisableVertexAttribArray(0);

		glDisableVertexAttribArray(1);

		glDisableVertexAttribArray(2);

		// unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}