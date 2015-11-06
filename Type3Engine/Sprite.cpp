#include "Sprite.h"
#include "Vertex.h"
#include <cstddef>
#include "ResourceManager.h"

namespace Type3Engine
{

	Sprite::Sprite()
	{
		_vboID = 0;
	}


	Sprite::~Sprite()
	{
		if (_vboID != 0)
		{
			glDeleteBuffers(1, &_vboID);
		}
	}

	void Sprite::init(float x, float y, float width, float height, std::string texturePath)
	{
		_x = x;
		_y = y;
		_width = width;
		_height = height;

		_texture = ResourceManager::getTexture(texturePath);


		if (_vboID == 0)
		{
			glGenBuffers(1, &_vboID);

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


		glBindBuffer(GL_ARRAY_BUFFER, _vboID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	void Sprite::draw()
	{

		// bind the texture
		glBindTexture(GL_TEXTURE_2D, _texture.id);// dont want to unbind this

		// bind the buffer object
		glBindBuffer(GL_ARRAY_BUFFER, _vboID);

		// tell opengl that we want to use the first attribute array
		glEnableVertexAttribArray(0);

		// This is our position attribute pointer, last value is the byte offset before the value is used in the struct
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		// this is our pixel attribute pointer;
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, colour));
		//this is out UV attribute pointer;
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		// draw our 6 verticies
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// disable the vertex attrib array
		glDisableVertexAttribArray(0);

		// unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

}