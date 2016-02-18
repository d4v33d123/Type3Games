#include "AnimatedSprite.h"
#include "SDL.h"

namespace T3E
{

	AnimatedSprite::AnimatedSprite()
	{
		vboID_ = 0;
	}


	AnimatedSprite::~AnimatedSprite()
	{
		if (vboID_ != 0)
		{
			glDeleteBuffers(1, &vboID_);
		}
	}

	void AnimatedSprite::init(float x, float y, float width, float height, std::string texturePath, float TileWidth, float TileHeight, int numSprites)
	{
		x_ = x;
		y_ = y;
		width_ = width;
		height_ = height;
		numSprites_ = numSprites;
		animPosition_ = 0;
		animSpeed_ = 0.7;
		tileHeight_ = TileHeight;
		tileWidth_ = TileWidth;
		animEnd_ = 20;
		//tileSheet_.init(ResourceManager::getTexture(texturePath),glm::ivec2(TileWidth, TileHeight));
		texture_ = ResourceManager::getTexture(texturePath);


		if (vboID_ == 0)
		{
			glGenBuffers(1, &vboID_);

		}

		Vertex vertexData[6];
		
		//top left

		vertexData[0].setPosition(x, y + height);
		vertexData[0].setUV(TileWidth*animPosition_,  TileHeight);
		//bottom left

		vertexData[1].setPosition(x, y);
		vertexData[1].setUV(TileWidth*animPosition_, 0);

		//bottom right
		vertexData[2].setPosition(x + width, y);
		vertexData[2].setUV(TileWidth*animPosition_ + TileWidth, 0);
		//bottom right
		vertexData[3].setPosition(x + width, y);
		vertexData[3].setUV(TileWidth*animPosition_ + TileWidth, 0);
		//top right
		vertexData[4].setPosition(x + width, y + height);
		vertexData[4].setUV(TileWidth*animPosition_ + TileWidth, TileHeight);
		//top left
		vertexData[5].setPosition(x, y + height);
		vertexData[5].setUV(TileWidth*animPosition_,  TileHeight);

		for (int i = 0; i < 6; i++)
		{
			vertexData[i].setColour(255, 255, 255, 255);
		}

		//vertexData[1].setColour(0, 0, 255, 255);


		//vertexData[4].setColour(0, 255, 0, 255);


		glBindBuffer(GL_ARRAY_BUFFER, vboID_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	void AnimatedSprite::draw()
	{

		// bind the texture
		glBindTexture(GL_TEXTURE_2D, texture_.id);// dont want to unbind this

		// bind the buffer object
		glBindBuffer(GL_ARRAY_BUFFER, vboID_);

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

	void AnimatedSprite::Update(float dTime)
	{
		int oldAnim = animPosition_;

		animCount_ += dTime*animSpeed_;
		//animPosition_ = (int)(fps_ * animCount_);
		if(animCount_ >= animEnd_)
		{
			animPosition_++;
			animCount_ = 0; 
		}
		
		if (animPosition_ != oldAnim)
		{
			Vertex vertexData[6];
		
			//top left

			vertexData[0].setPosition(x_, y_ + height_);
			vertexData[0].setUV(  tileHeight_, tileWidth_*animPosition_);
			//bottom left

			vertexData[1].setPosition(x_, y_);
			vertexData[1].setUV(0, tileWidth_*animPosition_);

			//bottom right
			vertexData[2].setPosition(x_ + width_, y_);
			vertexData[2].setUV(0, tileWidth_*animPosition_ + tileWidth_);
			//bottom right
			vertexData[3].setPosition(x_ + width_, y_);
			vertexData[3].setUV(0, tileWidth_*animPosition_ + tileWidth_);
			//top right
			vertexData[4].setPosition(x_ + width_, y_ + height_);
			vertexData[4].setUV(  tileHeight_, tileWidth_*animPosition_ + tileWidth_);
			//top left
			vertexData[5].setPosition(x_, y_ + height_);
			vertexData[5].setUV(  tileHeight_, tileWidth_*animPosition_);
			
			for (int i = 0; i < 6; i++)
			{
				vertexData[i].setColour(255, 255, 255, 255);
			}

			glBindBuffer(GL_ARRAY_BUFFER, vboID_);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		
		if(animPosition_ >= numSprites_)
		{
			animPosition_ = 0;
		} 
		
	}

}