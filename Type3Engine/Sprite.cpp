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

	void Sprite::init(float x, float y, float width, float height, std::string texturePath, float TileWidth, float TileHeight, int fps)
	{
		x_ = x;
		y_ = y;
		width_ = width;
		height_ = height;
		fps_ = fps;
		animationPosition_ = 0;
		tileHeight_ = TileHeight;
		tileWidth_ = TileWidth;

		tileSheet_.init(ResourceManager::getTexture(texturePath),glm::ivec2(TileWidth, TileHeight));


		if (vboID_ == 0)
		{
			glGenBuffers(1, &vboID_);

		}

		Vertex vertexData[6];
		//top right

		vertexData[0].setPosition(x + width, y + height);
		vertexData[0].setUV(TileWidth*animationPosition_ + TileWidth, TileHeight*animationPosition_ + TileHeight);
		//top left

		vertexData[1].setPosition(x, y + height);
		vertexData[1].setUV(TileWidth*animationPosition_, TileHeight*animationPosition_ + TileHeight);
		//bottom left

		vertexData[2].setPosition(x, y);
		vertexData[2].setUV(TileWidth*animationPosition_, TileHeight*animationPosition_);

		//bottom left
		vertexData[3].setPosition(x, y);
		vertexData[3].setUV(TileWidth*animationPosition_, TileHeight*animationPosition_);
		//bottom right
		vertexData[4].setPosition(x + width, y);
		vertexData[4].setUV(TileWidth*animationPosition_ + TileWidth, TileHeight*animationPosition_);
		//top right
		vertexData[5].setPosition(x + width, y + height);
		vertexData[5].setUV(TileWidth*animationPosition_ + TileWidth, TileHeight*animationPosition_ + TileHeight);

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
		glBindTexture(GL_TEXTURE_2D, tileSheet_.texture.id);// dont want to unbind this

	

		// draw our 6 verticies
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// disable the vertex attrib array
		glDisableVertexAttribArray(0);

		glDisableVertexAttribArray(1);

		glDisableVertexAttribArray(2);

		// unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Sprite::Update(float deltaTime)
	{
		int oldAnim = animationPosition_;

		counter += deltaTime;
		animationPosition_ = (int)(fps_ * counter);
		
		if (animationPosition_ != oldAnim)
		{
			Vertex vertexData[6];
			//top right

			//vertexData[0].setPosition(x_ + width_, y_ + height_);
			vertexData[0].setUV(tileWidth_*animationPosition_ + tileWidth_, tileHeight_*animationPosition_ + tileHeight_);
			//top left

			//vertexData[1].setPosition(x, y + height_);
			vertexData[1].setUV(tileWidth_*animationPosition_, tileHeight_*animationPosition_ + tileHeight_);
			//bottom left

			//vertexData[2].setPosition(x, y);
			vertexData[2].setUV(tileWidth_*animationPosition_, tileHeight_*animationPosition_);

			//bottom left
			//vertexData[3].setPosition(x, y);
			vertexData[3].setUV(tileWidth_*animationPosition_, tileHeight_*animationPosition_);
			//bottom right
			//vertexData[4].setPosition(x + width_, y);
			vertexData[4].setUV(tileWidth_*animationPosition_ + tileWidth_, tileHeight_*animationPosition_);
			//top right
			//vertexData[5].setPosition(x + width_, y + height_);
			vertexData[5].setUV(tileWidth_*animationPosition_ + tileWidth_, tileHeight_*animationPosition_ + tileHeight_);

			glBindBuffer(GL_ARRAY_BUFFER, vboID_);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

		}
		
	}

}