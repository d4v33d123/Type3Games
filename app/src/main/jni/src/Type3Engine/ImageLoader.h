#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif//__ANDROID__
#include "glTexture.h"
#include <string>
#include "picoPNG.h"
#include "IOManager.h"
#include "errors.h"

#include <sstream>//remove if you can fix 'to_string' is not a member of 'std'

namespace T3E
{	
	class ImageLoader
	{	
	public:
	
		/**
		*
		* Loads a PNG into the game
		*
		* @param [in] The string containing the file path that is going to be accessed 
		*/
		static GLTexture loadPNG(std::string filePath);
	};
}

#endif