#include "SpriteBatch.h"
#include <algorithm>

namespace T3E
{
	SpriteBatch::SpriteBatch() : vbo_(0), vao_(0)
	{
	}


	SpriteBatch::~SpriteBatch()
	{
	}


	void SpriteBatch::init()
	{
		createVertexArray();
	}

	void SpriteBatch::begin(GlyphSortType sortType /*= GlyphSortType::TEXTURE*/)
	{
		sortType_ = sortType;
		renderBatches_.clear();
		for (int i = 0; i < glyphs_.size(); i++)
		{
			delete glyphs_[i];
		}
		glyphs_.clear();
	}

	void SpriteBatch::end()
	{
		sortGlyphs();
		createRenderBatches();
	}

	void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const Colour& colour)
	{
		Glyph* newGlyph = new Glyph;
		newGlyph->texture = texture;
		newGlyph->depth = depth;

		newGlyph->topLeft.colour = colour;
		newGlyph->topLeft.setPosition(destRect.x, destRect.y + destRect.w);
		newGlyph->topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

		newGlyph->bottomLeft.colour = colour;
		newGlyph->bottomLeft.setPosition(destRect.x, destRect.y );
		newGlyph->bottomLeft.setUV(uvRect.x, uvRect.y );

		newGlyph->bottomRight.colour = colour;
		newGlyph->bottomRight.setPosition(destRect.x + destRect.z, destRect.y );
		newGlyph->bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y );

		newGlyph->topRight.colour = colour;
		newGlyph->topRight.setPosition(destRect.x + destRect.z, destRect.y + destRect.w);
		newGlyph->topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

		glyphs_.push_back(newGlyph);
	}

	void SpriteBatch::renderBatch()
	{
		glBindVertexArray(vao_);

		for (int i = 0; i < renderBatches_.size(); i++)
		{
			glBindTexture(GL_TEXTURE_2D, renderBatches_[i].texture);

			glDrawArrays(GL_TRIANGLES, renderBatches_[i].offSet, renderBatches_[i].numVerticies);

		}

		glBindVertexArray(0);
	}

	void SpriteBatch::createRenderBatches()
	{
		std::vector <Vertex> verticies;
		//reserving the size to speed up the process, makes stepping through faster
		verticies.resize(glyphs_.size() * 6);

		if (glyphs_.empty())
		{
			return;
		}
		int offset = 0;
		int cv = 0; //current Vertex

		// more efficent than creating a new batch and adding a copy of it to the vector of render batches
		renderBatches_.emplace_back(offset, 6, glyphs_[0]->texture);
		// adds to cv after the line of code is run
		verticies[cv++] = glyphs_[0]->topLeft;
		verticies[cv++] = glyphs_[0]->bottomLeft;
		verticies[cv++] = glyphs_[0]->bottomRight;
		verticies[cv++] = glyphs_[0]->bottomRight;
		verticies[cv++] = glyphs_[0]->topRight;
		verticies[cv++] = glyphs_[0]->topLeft;
		offset += 6;

		for (int cg = 1; cg < glyphs_.size(); cg++) // current glyph
		{
			if (glyphs_[cg]->texture != glyphs_[cg - 1]->texture)
			{
				renderBatches_.emplace_back(0, 6, glyphs_[cg]->texture);
			}
			else
			{
				renderBatches_.back().numVerticies += 6;
			}
			verticies[cv++] = glyphs_[cg]->topLeft;
			verticies[cv++] = glyphs_[cg]->bottomLeft;
			verticies[cv++] = glyphs_[cg]->bottomRight;
			verticies[cv++] = glyphs_[cg]->bottomRight;
			verticies[cv++] = glyphs_[cg]->topRight;
			verticies[cv++] = glyphs_[cg]->topLeft;
			offset += 6;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		//orphan the buffer
		glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
		//upload the data
		glBufferSubData(GL_ARRAY_BUFFER, 0, verticies.size() * sizeof(Vertex), verticies.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SpriteBatch::createVertexArray()
	{
		if (vao_ == 0)
		{
			glGenVertexArrays(1, &vao_);
		}
		glBindVertexArray(vao_);

		if (vbo_ == 0)
		{
			glGenBuffers(1, &vbo_);
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		// tell opengl that we want to use the first attribute array
		glEnableVertexAttribArray(0);

		glEnableVertexAttribArray(1);

		glEnableVertexAttribArray(2);

		// This is our position attribute pointer, last value is the byte offset before the value is used in the struct
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		// this is our pixel attribute pointer;
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, colour));
		//this is out UV attribute pointer;
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		glBindVertexArray(0);
		
	}

	void SpriteBatch::sortGlyphs()
	{
		switch (sortType_)
		{
		case GlyphSortType::BACK_TO_FRONT:
			std::stable_sort(glyphs_.begin(), glyphs_.end(), compareBackToFront);
			break;
		case GlyphSortType::FRONT_TO_BACK:
			std::stable_sort(glyphs_.begin(), glyphs_.end(), compareFrontToBack);
			break;
		case GlyphSortType::TEXTURE:
			std::stable_sort(glyphs_.begin(), glyphs_.end(), compareTexture);
			break;
		}
		
	}

	bool SpriteBatch::compareFrontToBack(Glyph* a, Glyph* b)
	{
		return(a->depth < b->depth);
	}
	bool SpriteBatch::compareBackToFront(Glyph* a, Glyph* b)
	{
		return(a->depth > b->depth);
	}
	bool SpriteBatch::compareTexture(Glyph* a, Glyph* b)
	{
		return(a->texture < b->texture);
	}
}
