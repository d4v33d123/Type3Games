#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <vector>
#include <string>
#include "glTexture.h"

namespace T3E
{
	class TextRenderer
	{
	public:
		TextRenderer();
		~TextRenderer();

		void init();
		void setScreenSize( int width, int height ) { width_ = width; height_ = height; }

		void putString( std::string str, float x, float y, unsigned int size_pixels );
		void putChar( unsigned char c, float x, float y, unsigned int size_pixels );

		void render();

	private:
		int width_;
		int height_;

		GLTexture bitmap_font_;
		GLint texture_sampler_;
		GLuint shader_program_;
		GLuint vertex_shader_;
		GLuint pixel_shader_;
		GLuint vbo_;

		void pushVert( float x, float y, float u, float v );
		std::vector<GLfloat> verts_;
	};
}

#endif