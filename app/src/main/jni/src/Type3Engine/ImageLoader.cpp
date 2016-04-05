#include "ImageLoader.h"

namespace T3E
{
	GLuint GLTexture::numTextures = 0;

	//workaround for 'to_string' is not a member of 'std'
	template <typename T>
	std::string to_string(T value)
	{
		std::ostringstream os ;
		os << value ;
		return os.str() ;
	}
	
	GLTexture ImageLoader::loadPNG(std::string filePath)
	{
		// create a blank texture
		GLTexture texture = {};

		// create variables for handling the decodePNG
		std::vector<unsigned char> in;
		std::vector<unsigned char> out;
		unsigned long width, height;

		
		if (IOManager::readBinaryToBuffer(filePath, in) == false)
		{
			fatalError("Failed to load PNG file to buffer! " + filePath);
		}
		
		
		int errorCode = decodePNG(out, width, height, &(in[0]), in.size());
		
		if (errorCode != 0)
		{
			fatalError("decodePNG failed with error: " + to_string(errorCode));
		}

		// set the width and height 
		texture.width = width;
		texture.height = height;
		texture.unit = GLTexture::numTextures;
		GLTexture::numTextures++;

		// generate the texture
		glGenTextures(1, &(texture.id));

		glActiveTexture( GL_TEXTURE0 + texture.unit );
		glBindTexture( GL_TEXTURE_2D, texture.unit );

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(out[0]));

		// set the texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);


		SDL_Log("Loaded texture %s id %i unit %i", filePath.c_str(), texture.id, texture.unit );

		return texture;
	}
}