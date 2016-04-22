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

        /**
		*
		* Get the font testure and compile shaders
		*
		*/
		void init();
        
        /**
		*
		* Set the screen dimension for appropriate rendering
		*
        * @param [in] width of the screen (pixel resolution)
		* @param [in] height of the screen (pixel resolution)
        *
		*/
		void setScreenSize( int width, int height ) { width_ = width; height_ = height; }

		// Coordinates for renderering text are specified as an opengl screen coord
		// -1 -1 is the bottom left of the screen, 1 1 is the top right
		// Coords are where the top left of the text will be placed
		// size_pixels is the number of pixels wide/tall each character is
		// TODO: it may be better to specify text size in screen coord too, so the text is always covers
		// the same amount of space on screen no matter the resolution

        /**
		*
		* Add a number to be rendered
		*
        * @param [in] the number to be rendered
		* @param [in] the amount of space to write the number into
        * @param [in] x coordinate of the top left corner of the text. -1 -1 is the bottom left of the screen, 1 1 is the top right
        * @param [in] y coordinate of the top left corner of the text. -1 -1 is the bottom left of the screen, 1 1 is the top right
        * @param [in] the number of pixels wide/tall each character is
        *
		*/
		void putNumber( int num, unsigned padding, float x, float y, unsigned size_pixels );
        
        /**
		*
		* Add a string of characters to be rendered
		*
        * @param [in] the string to be rendered
        * @param [in] x coordinate of the top left corner of the text. -1 -1 is the bottom left of the screen, 1 1 is the top right
        * @param [in] y coordinate of the top left corner of the text. -1 -1 is the bottom left of the screen, 1 1 is the top right
        * @param [in] the number of pixels wide/tall each character is
        *
		*/
		void putString( std::string str, float x, float y, unsigned int size_pixels );
        
        /**
		*
		* Add a single character to be rendered
		*
        * @param [in] the char to be rendered
        * @param [in] x coordinate of the top left corner of the text. -1 -1 is the bottom left of the screen, 1 1 is the top right
        * @param [in] y coordinate of the top left corner of the text. -1 -1 is the bottom left of the screen, 1 1 is the top right
        * @param [in] the number of pixels wide/tall each character is
        *
		*/
		void putChar( unsigned char c, float x, float y, unsigned int size_pixels );

        /**
		*
		* Render the loaded text and unload it
        *
        * The text to be rendered must be loaded again after calling this function
        *
		*/
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