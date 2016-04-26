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
	height_(0)
	{
	}

	TextRenderer::~TextRenderer()
	{
	}

	void TextRenderer::init()
	{
		bitmap_font_ = ResourceManager::getTexture( "textures/font.png" );

		// Create the shader, should probably be using the GLSLshader class here but I don't understand it yet...
		shader_program_ = glCreateProgram();
		vertex_shader_ = glCreateShader( GL_VERTEX_SHADER );
		if( vertex_shader_ == 0 )
		{
			fatalError("Could not create font vertex shader");
		}
		pixel_shader_ = glCreateShader( GL_FRAGMENT_SHADER );
		if( pixel_shader_ == 0 )
		{
			fatalError("Could not create font pixel shader");
		}

		// Compile the vertex shader
		{
			std::vector<char> source;
			if( !IOManager::readTextToBuffer( "shaders/font_vs.txt", source ) ) fatalError("Failed to open font vertex shader");

			const char* sourcePtr = source.data();
			glShaderSource( vertex_shader_, 1, &sourcePtr, NULL );
			glCompileShader( vertex_shader_ );

			// Check the shader compiled ok
			GLint success = 0;
			glGetShaderiv( vertex_shader_, GL_COMPILE_STATUS, &success );
			if( success != GL_TRUE )
			{
				GLint maxLength = 0;
				glGetShaderiv( vertex_shader_, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<char> errorLog(maxLength);
				glGetShaderInfoLog( vertex_shader_, maxLength, &maxLength, &errorLog[0]);

				glDeleteShader( vertex_shader_); //dont leak the shader

				SDL_Log( "%s\n", &(errorLog[0]) );
				fatalError("Shader font vertex failed to compile");				
			}
		}
		// Compile the pixel shader
		{
			std::vector<char> source;
			if( !IOManager::readTextToBuffer( "shaders/font_ps.txt", source ) ) fatalError("Failed to open font pixel shader");

			const char* sourcePtr = source.data();
			glShaderSource( pixel_shader_, 1, &sourcePtr, NULL );
			glCompileShader( pixel_shader_ );

			// Check the shader compiled ok
			GLint success = 0;
			glGetShaderiv( pixel_shader_, GL_COMPILE_STATUS, &success );
			if( success != GL_TRUE )
			{
				GLint maxLength = 0;
				glGetShaderiv( pixel_shader_, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<char> errorLog(maxLength);
				glGetShaderInfoLog( pixel_shader_, maxLength, &maxLength, &errorLog[0]);

				glDeleteShader( pixel_shader_); //dont leak the shader

				SDL_Log( "%s\n", &(errorLog[0]) );
				fatalError("Shader font pixel failed to compile");				
			}
		}

		// Add the attributes
		glBindAttribLocation( shader_program_, 0, "vPosition" );
		glBindAttribLocation( shader_program_, 1, "vTexCoord" );

		// link the shaders
		glAttachShader( shader_program_, vertex_shader_ );
		glAttachShader( shader_program_, pixel_shader_ );
		glLinkProgram( shader_program_ );

		GLint success = GL_FALSE;
		glGetProgramiv( shader_program_, GL_LINK_STATUS, &success );
		if( success == GL_FALSE )
		{
			GLint maxLength = 0;
			glGetProgramiv( shader_program_, GL_INFO_LOG_LENGTH, &maxLength );
			std::vector<char> errorLog( maxLength );
			glGetProgramInfoLog( shader_program_, maxLength, &maxLength, &errorLog[0] );

			glDeleteProgram( shader_program_ ); // dont leak the program
			glDeleteShader( vertex_shader_ ); // or the vertex shader
			glDeleteShader( pixel_shader_ ); // or the pixel shader

			SDL_Log("%s\n", &(errorLog[0]));
			fatalError("Program failed to link shaders!");
		}

		texture_sampler_ = glGetUniformLocation( shader_program_, "font_bitmap" );
		if( texture_sampler_ == 0xFFFFFFFF ) { // GL_INVALID_INDEX is not defined in GLES
			fatalError("Uniform 'font_bitmap' not found in shader! (or it is unused)");
		}

		// Cleanup shaders after successful compile and link		
		glDetachShader( shader_program_, vertex_shader_ );
		glDetachShader( shader_program_, pixel_shader_ );
		glDeleteShader( vertex_shader_ );
		glDeleteShader( pixel_shader_ );

		glUseProgram( shader_program_ );
	    glGenBuffers( 1, &vbo_ );
		glBindBuffer( GL_ARRAY_BUFFER, vbo_ );
		glUniform1i( texture_sampler_, bitmap_font_.unit );
	}

	void TextRenderer::putNumber( int num, unsigned padding, float x, float y, unsigned size_pixels, float alpha )
	{
		int num_digits = 1;
		std::string num_str;

		if( num < 0 ) {
			num_str += '-';
			num *= -1;
		}

		while( std::pow(10, num_digits) < num ) num_digits++;
		while( --padding > num_digits ) num_str += ' ';

		int prev_digit = 0;
		while( num_digits-- > 0 )
		{
			int digit = num / (int)std::pow(10, num_digits ) ;
			char c = 48 + digit - prev_digit;
			if( c == 58 ) { num_str += '1'; num_str += '0'; }
			else { num_str += c; }
			prev_digit = digit * 10;
		}

		putString( num_str, x, y, size_pixels, alpha );
	}

	void TextRenderer::putString( std::string str, float x, float y, unsigned int size_pixels, float alpha )
	{
		float char_width = (size_pixels / (float)width_) * 2.0f;
		int chars_this_line = 0;

		for( size_t i = 0; i < str.size(); ++i )
		{
			// If it's a new line, move down and back
			if( str[i] == '\n' ) {
				y += (size_pixels / (float)height_) * -2.05f;
				x -= char_width * (chars_this_line + 1);
				chars_this_line = 0;
				continue;
			}

			putChar( str[i], x + char_width * i, y, size_pixels, alpha );
			chars_this_line++;
		}
	}

	void TextRenderer::putChar( unsigned char c, float x, float y, unsigned int size_pixels, float alpha )
	{			
		static float tex_width = 1.0f / 16.0f;
		static float tex_height = 1.0f / 16.0f;
		float tex_x = (c % 16) * tex_width;
		float tex_y = (c / 16) * tex_height;

		float width = (size_pixels / (float)width_) * 2.0f;
		float height = (size_pixels / (float)height_) * -2.0f;

		pushVert( x, 			y, 			tex_x, 				tex_y, 				alpha );
		pushVert( x, 			y + height, tex_x, 				tex_y + tex_height, alpha );
		pushVert( x + width, 	y + height, tex_x + tex_width, 	tex_y + tex_height, alpha );
		pushVert( x, 			y, 			tex_x, 				tex_y, 				alpha );
		pushVert( x + width, 	y + height, tex_x + tex_width, 	tex_y + tex_height, alpha );
		pushVert( x + width, 	y, 			tex_x + tex_width, 	tex_y, 				alpha );
	}

	void TextRenderer::render()
	{
		// Bind the shader and verts
		glUseProgram( shader_program_ );
		glBindBuffer( GL_ARRAY_BUFFER, vbo_ );
		glBufferData( GL_ARRAY_BUFFER, sizeof(GLfloat) * verts_.size(), verts_.data(), GL_DYNAMIC_DRAW );

    	// Sent the texture and set properties
		glActiveTexture( GL_TEXTURE0 + bitmap_font_.unit );	
    	glBindTexture( GL_TEXTURE_2D, bitmap_font_.unit );

	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	    // Specify the vertex layout
		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, 0 );
		glEnableVertexAttribArray( 1 );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(2*sizeof(GLfloat)) );

		glDrawArrays( GL_TRIANGLES, 0, verts_.size() / 5 );

		// Clear the array ready for next time
		verts_.clear();
	}

	void TextRenderer::pushVert( float x, float y, float u, float v, float a )
	{
		verts_.push_back( x );
		verts_.push_back( y );
		verts_.push_back( u );
		verts_.push_back( v );
		verts_.push_back( a );
	}
}