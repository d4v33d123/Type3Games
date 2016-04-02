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
		
		glBindBuffer(GL_ARRAY_BUFFER, vboID_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Sprite::draw()
	{
		// bind the texture is not already bound
		// NOTE: this doesn't take into account non-sprite bind calls so could forget to bind in certain cases
		// But the way the code is structured curretly we should be fine
		if( boundTextureUnit_ != texture_.unit ) {
			glBindTexture(GL_TEXTURE_2D, texture_.unit);
			boundTextureUnit_ = texture_.unit;
		}

		// bind the buffer object
		glBindBuffer(GL_ARRAY_BUFFER, vboID_);

		// This is our position attribute pointer, last value is the byte offset before the value is used in the struct
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		//this is out UV attribute pointer;
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		// draw our 6 verticies
		glDrawArrays(GL_TRIANGLES, 0, 6); // crash here from start menu after tutorial etc
	}
}