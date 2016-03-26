#include "TextRenderer.h"
#include "ResourceManager.h"
#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif //__ANDROID__
#include "errors.h"

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
		SDL_Log("Bitmap font %i %i %i", bitmap_font_.id, bitmap_font_.width, bitmap_font_.height );

		// Create the shader, should probably be using the GLSLshader class here but I don't understand it yet...
		shader_program_ = glCreateProgram();
		vertex_shader_ = glCreateShader( GL_VERTEX_SHADER );
		if( vertex_shader_ == 0 )
		{
			fatalError("Could not create font vertex shader");
		}
		pixel_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
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

		GLuint isLinked = 0;
		glGetProgramiv( shader_program_, GL_LINK_STATUS, (int*)&isLinked );
		if( isLinked == GL_FALSE )
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


		// bind the shader
		glUseProgram( shader_program_ );
		//glEnableVertexAttribArray( 0 );
		//glEnableVertexAttribArray( 1 );

		// Test geometry

	    glGenBuffers( 1, &vbo_ );
		glBindBuffer( GL_ARRAY_BUFFER, vbo_ );

		glActiveTexture( GL_TEXTURE0 + bitmap_font_.id );	
    	glBindTexture( GL_TEXTURE_2D, bitmap_font_.id );
		glUniform1i( texture_sampler_, bitmap_font_.id );
		SDL_Log("Texture ID %i", bitmap_font_.id );

		GLfloat verts[] = 
		{
			 0.0f,  0.5f, 0.5f, 1.0f, 
			-0.5f, -0.5f, 0.0f, 0.0f,
			 0.5f, -0.5f, 1.0f, 0.0f
		};

	    // Push the verts to the GPU
		glBindBuffer( GL_ARRAY_BUFFER, vbo_ );
		glBufferData( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW );
	}

	void TextRenderer::putChar( unsigned char c, float x, float y, unsigned int size_pixels )
	{

	}

	void TextRenderer::render()
	{
		glUseProgram( shader_program_ );
		glBindBuffer( GL_ARRAY_BUFFER, vbo_ );

		glActiveTexture( GL_TEXTURE0 + bitmap_font_.id );	
    	glBindTexture( GL_TEXTURE_2D, bitmap_font_.id );


		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, 0 );
		glEnableVertexAttribArray( 1 );
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(2*sizeof(GLfloat)) );

		glDrawArrays( GL_TRIANGLES, 0, 3 );
	}
}