#include "Sprite.h"

namespace T3E
{
	GLuint Sprite::boundTextureUnit_ = 0;

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

	void Sprite::init(float x, float y, float width, float height, std::string texturePath,float tileX, float tileY, float tileWidth, float tileHeight)
	{
		x_ = x;
		y_ = y;
		width_ = width;
		height_ = height;
		tileX_ = tileX;
		tileY_ = tileY;
		tileHeight_ = tileHeight;
		tileWidth_ = tileWidth;
		
		texture_ = ResourceManager::getTexture(texturePath);

		if (vboID_ == 0)
		{
			glGenBuffers(1, &vboID_);
		}
		
		Vertex vertexData[6];
		
		//top left
		vertexData[0].setPosition(x_, y_ + height_);
		vertexData[0].setUV(tileX_, tileY_);	
		
		//bottom left
		vertexData[1].setPosition(x_, y_);
		vertexData[1].setUV(tileX_ + tileWidth_, tileY_);
		
		//bottom right
		vertexData[2].setPosition(x_ + width_, y_);
		vertexData[2].setUV(tileX_ + tileWidth_, tileY_ + tileHeight_);
		
		//bottom right
		vertexData[3].setPosition(x_ + width_, y_);
		vertexData[3].setUV(tileX_ + tileWidth_, tileY_ + tileHeight_);	
		
		//top right
		vertexData[4].setPosition(x_ + width_, y_ + height_);
		vertexData[4].setUV(tileX_, tileY_ + tileHeight_);
		
		//top left
		vertexData[5].setPosition(x_, y_ + height_);
		vertexData[5].setUV(tileX_, tileY_);
				
		//set vertex colours
		for (int i = 0; i < 6; i++)
		{
			//vertexData[i].setColour(255, 255, 255, 255);
		}
		
		glBindBuffer(GL_ARRAY_BUFFER, vboID_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Sprite::draw()
	{
		// bind the texture
		if( boundTextureUnit_ != texture_.unit ) {
			glBindTexture(GL_TEXTURE_2D, texture_.unit); // dont want to unbind this
			boundTextureUnit_ = texture_.unit;
		}

		// bind the buffer object
		glBindBuffer(GL_ARRAY_BUFFER, vboID_);

		// tell opengl that we want to use the first attribute array
		glEnableVertexAttribArray(0);

		// This is our position attribute pointer, last value is the byte offset before the value is used in the struct
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		// this is our pixel attribute pointer;
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, colour));
		//this is our UV attribute pointer;
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		// draw our 6 verticies
		glDrawArrays(GL_TRIANGLES, 0, 6); // crash here from start menu after tutorial etc

		// disable the vertex attrib array
		glDisableVertexAttribArray(0);

		// unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}