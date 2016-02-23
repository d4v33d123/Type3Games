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
	
	//UV COORDS ARE SWAPPED!!! invert width with height and x with y for tiling, position in spritesheet etc
	void AnimatedSprite::init(float x, float y, float width, float height, std::string texturePath, float tileX, float tileY, float tileWidth, float tileHeight, int numSprites)
	{
		x_ = x;
		y_ = y;
		width_ = width;
		height_ = height;
		numSprites_ = numSprites;
		animPosition_ = 0;
		animSpeed_ = 0.7;
		tileX_ = tileX;
		tileY_ = tileY;
		tileHeight_ = tileHeight;
		tileWidth_ = tileWidth;
		animEnd_ = 20;
		//tileSheet_.init(ResourceManager::getTexture(texturePath),glm::ivec2(TileWidth, TileHeight));
		texture_ = ResourceManager::getTexture(texturePath);

		if (vboID_ == 0)
		{
			glGenBuffers(1, &vboID_);

		}

		Vertex vertexData[6];
		
		//UV COORDS ARE SWAPPED!!!
		//top left
		vertexData[0].setPosition(x_, y_ + height_);
		vertexData[0].setUV(tileX_ + 0 , tileY_ + tileWidth_*animPosition_);		
		//bottom left
		vertexData[1].setPosition(x_, y_);
		vertexData[1].setUV(tileX_ + tileHeight_, tileY_ + tileWidth_*animPosition_ );
		//bottom right
		vertexData[2].setPosition(x_ + width_, y_);
		vertexData[2].setUV(tileX_ + tileHeight_, tileY_ + tileWidth_*animPosition_+ tileWidth_);
		
		//bottom right
		vertexData[3].setPosition(x_ + width_, y_);
		vertexData[3].setUV(tileX_ + tileHeight_, tileY_ + tileWidth_*animPosition_+ tileWidth_);	
		//top right
		vertexData[4].setPosition(x_ + width_, y_ + height_);
		vertexData[4].setUV(tileX_ + 0, tileY_ + tileWidth_*animPosition_+ tileWidth_);		
		//top left
		vertexData[5].setPosition(x_, y_ + height_);
		vertexData[5].setUV(tileX_ + 0, tileY_ + tileWidth_*animPosition_);

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
		
			//UV COORDS ARE SWAPPED!!!
			//top left
			vertexData[0].setPosition(x_, y_ + height_);
			vertexData[0].setUV(tileX_ + 0 , tileY_ + tileWidth_*animPosition_);		
			//bottom left
			vertexData[1].setPosition(x_, y_);
			vertexData[1].setUV(tileX_ + tileHeight_, tileY_ + tileWidth_*animPosition_ );
			//bottom right
			vertexData[2].setPosition(x_ + width_, y_);
			vertexData[2].setUV(tileX_ + tileHeight_, tileY_ + tileWidth_*animPosition_+ tileWidth_);
			
			//bottom right
			vertexData[3].setPosition(x_ + width_, y_);
			vertexData[3].setUV(tileX_ + tileHeight_, tileY_ + tileWidth_*animPosition_+ tileWidth_);	
			//top right
			vertexData[4].setPosition(x_ + width_, y_ + height_);
			vertexData[4].setUV(tileX_ + 0, tileY_ + tileWidth_*animPosition_+ tileWidth_);		
			//top left
			vertexData[5].setPosition(x_, y_ + height_);
			vertexData[5].setUV(tileX_ + 0, tileY_ + tileWidth_*animPosition_);
			
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