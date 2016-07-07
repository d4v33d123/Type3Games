#include "TextRenderer.h"
#include "ResourceManager.h"
#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif //__ANDROID__
#include "errors.h"
#include <cmath>

namespace T3E
{
	TextRenderer::TextRenderer() :
	width_(0),
	height_(0),
	texture_sampler_(0),
	vbo_(0)
	{
	}

	TextRenderer::~TextRenderer()
	{
	}

	void TextRenderer::init()
	{
		bitmap_font_ = ResourceManager::getTexture( "textures/font.png" );
	    glGenBuffers( 1, &vbo_ );

		/* Font Shader: draws text */
		font_shader_.compileShaders("shaders/font_vs.txt", "shaders/font_ps.txt");
		font_shader_.addAttribute("vPosition");
		font_shader_.addAttribute("vTexCoord");
		font_shader_.linkShaders();

		texture_sampler_ = font_shader_.getUniformLocation("font_bitmap");
	}

	void TextRenderer::destroy()
	{
		font_shader_.destroy();
		background_shader_.destroy();
	}

	void TextRenderer::putNumber( int num, unsigned padding, float x, float y, float char_size, float alpha )
	{
		int num_digits = 1;
		std::string num_str;

		if( num < 0 ) {
			num_str += '-';
			num *= -1;
		}

		while( std::pow(10, num_digits) <= num ) num_digits++;
		while( padding-- > num_digits ) num_str += ' ';

		int prev_digit = 0;
		while( num_digits-- > 0 )
		{
			int digit = num / (int)std::pow(10, num_digits ) ;
			char c = 48 + digit - prev_digit;
			if( c == 58 ) { num_str += '1'; num_str += '0'; }
			else { num_str += c; }
			prev_digit = digit * 10;
		}

		putString( num_str, x, y, char_size, alpha );
	}

	void TextRenderer::putString( std::string str, float x, float y, float char_size, float alpha )
	{
		//float char_width = (size_pixels / (float)width_) * 2.0f;
		int chars_this_line = 0;
		float line_height = 1.2f;
		float char_spacing = 0.9f;

		for( size_t i = 0; i < str.size(); ++i )
		{
			// If it's a new line, move down and back
			if( str[i] == '\n' ) {
				//y += (size_pixels / (float)height_) * -2.20f;
				y -= char_size * line_height;
				x -= char_size * (chars_this_line + 1) * char_spacing;
				chars_this_line = 0;
				continue;
			}

			putChar( str[i], x + char_size * i * char_spacing, y, char_size, alpha );
			chars_this_line++;
		}
	}

	void TextRenderer::putChar( unsigned char c, float x, float y, float char_size, float alpha )
	{			
		static float tex_width = 1.0f / 16.0f;
		static float tex_height = 1.0f / 16.0f;
		float tex_x = (c % 16) * tex_width;
		float tex_y = (c / 16) * tex_height;

		//float width = (size_pixels / (float)width_) * 2.0f;
		//float height = (size_pixels / (float)height_) * -2.0f;
		float width = char_size;
		float height = -char_size;

		pushCharVert( x, 			y, 			tex_x, 				tex_y, 				alpha );
		pushCharVert( x, 			y + height, tex_x, 				tex_y + tex_height, alpha );
		pushCharVert( x + width, 	y + height, tex_x + tex_width, 	tex_y + tex_height, alpha );
		pushCharVert( x, 			y, 			tex_x, 				tex_y, 				alpha );
		pushCharVert( x + width, 	y + height, tex_x + tex_width, 	tex_y + tex_height, alpha );
		pushCharVert( x + width, 	y, 			tex_x + tex_width, 	tex_y, 				alpha );
	}

	void TextRenderer::render()
	{
		// Bind the shader and verts
		font_shader_.use();
		glBindBuffer( GL_ARRAY_BUFFER, vbo_ );
		glUniform1i( texture_sampler_, bitmap_font_.unit );
		glBufferData( GL_ARRAY_BUFFER, sizeof(GLfloat) * char_verts_.size(), char_verts_.data(), GL_DYNAMIC_DRAW );

    	// Sent the texture and set properties
		glActiveTexture( GL_TEXTURE0 + bitmap_font_.unit );
    	glBindTexture( GL_TEXTURE_2D, bitmap_font_.unit );

	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	    // Specify the vertex layout
		//glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, 0 );
		//glEnableVertexAttribArray( 1 );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(2*sizeof(GLfloat)) );

		glDrawArrays( GL_TRIANGLES, 0, char_verts_.size() / 5 );

		// Clear the array ready for next time
		char_verts_.clear();
	}

	void TextRenderer::pushCharVert( float x, float y, float u, float v, float a )
	{
		char_verts_.push_back( x );
		char_verts_.push_back( y );
		char_verts_.push_back( u );
		char_verts_.push_back( v );
		char_verts_.push_back( a );
	}
}