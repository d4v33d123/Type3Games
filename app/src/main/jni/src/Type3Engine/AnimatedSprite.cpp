#include "AnimatedSprite.h"
#include "SDL.h"

namespace T3E
{

	AnimatedSprite::AnimatedSprite()
	{
		needRefresh_ = false;
		vboID_ = 0;
	}


	AnimatedSprite::~AnimatedSprite()
	{
		if (vboID_ != 0)
		{
			glDeleteBuffers(1, &vboID_);
		}
	}
	
	void AnimatedSprite::updateUVs()
	{
		Vertex vertexData[6];
			
		//UV COORDS ARE SWAPPED!!!
		//top left
		vertexData[0].setPosition(x_, y_ + height_);
		vertexData[0].setUV(tileX_ , tileY_ + tileWidth_*animPosition_);		
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
		vertexData[4].setUV(tileX_, tileY_ + tileWidth_*animPosition_+ tileWidth_);		
		//top left
		vertexData[5].setPosition(x_, y_ + height_);
		vertexData[5].setUV(tileX_, tileY_ + tileWidth_*animPosition_);

		glBindBuffer(GL_ARRAY_BUFFER, vboID_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	//UV COORDS ARE SWAPPED!!! invert width with height and x with y for tiling, position in spritesheet etc
	void AnimatedSprite::init(float x, float y, float width, float height, std::string texturePath,
		float tileX, float tileY, float tileWidth, float tileHeight, int numSprites, int framesPerLine)
	{
		x_ = x;
		y_ = y;
		width_ = width;
		height_ = height;
		numSprites_ = numSprites;
		animPosition_ = 0;
		animSpeed_ = 0.7;
		animCount_ = 0;
		tileX_ = tileX;
		originalTileX_ = tileX_;
		tileY_ = tileY;
		tileHeight_ = tileHeight;
		tileWidth_ = tileWidth;
		animEnd_ = 20;
		framesPerLine_ = framesPerLine;
		texture_ = ResourceManager::getTexture(texturePath);

		if (vboID_ == 0)
		{
			glGenBuffers(1, &vboID_);
		}

		updateUVs();
	}

	void AnimatedSprite::draw()
	{
		// bind the texture
		if( boundTextureUnit_ != texture_.unit ) {
			glBindTexture(GL_TEXTURE_2D, texture_.unit);
			boundTextureUnit_ = texture_.unit;
		}

		// bind the buffer object
		glBindBuffer(GL_ARRAY_BUFFER, vboID_);

		// This is our position attribute pointer, last value is the byte offset before the value is used in the struct
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		//this is our UV attribute pointer;
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		// draw our 6 verticies
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	bool AnimatedSprite::Update(float dTime)
	{
		int oldAnim = animPosition_;
		
		if(needRefresh_)
		{
			tileX_ = originalTileX_;
			animPosition_ = 0;
			needRefresh_ = false;
		}
		else
		{
			animCount_ += dTime*animSpeed_;
			//animPosition_ = (int)(fps_ * animCount_);
			if(animCount_ >= animEnd_)
			{
				animPosition_++;
				animCount_ = 0;
				if(animPosition_ % framesPerLine_ == 0 && animPosition_ < numSprites_)
					tileX_ += tileWidth_;
			}
		}
		
		if (animPosition_ != oldAnim)
		{
			if(animPosition_ >= numSprites_)//rendered last frame of loop
			{
				needRefresh_ = true;
				return true;
			}
			else
				updateUVs();
		}			
		
		return false;		
	}

}