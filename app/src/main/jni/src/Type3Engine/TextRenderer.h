#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

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
		void putChar( unsigned char c, float x, float y, unsigned int size_pixels );

		void render();

	private:
		int width_;
		int height_;

		GLTexture bitmap_font_;
		GLuint shader_program_;
		GLuint vertex_shader_;
		GLuint pixel_shader_;
		GLuint vbo_;
	};
}

#endif