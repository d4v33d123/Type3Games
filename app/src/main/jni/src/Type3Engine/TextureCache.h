#ifndef TEXTURECACHE_H
#define TEXTURECACHE_H

#include <map>
#include "glTexture.h"
#include "ImageLoader.h"
#include "SDL.h"

namespace T3E
{
	class TextureCache
	{
	public:
		TextureCache();
		~TextureCache();

        /**
		*
		* Get the texture info of the specified texture from the map.
        * if the texture is not currently in the map it will be added to it.
		*
        * @param [in] The string containing the file path to the png texture file
        *
		*/
		GLTexture getTexture(std::string texturePath);
        
		/**
		*
		* Call the clear() method of the std::map member
		*
        * Useful since android seems to not clear memory properly when exiting the program apparently (especially static objects)
        *
		*/
        void clear();

	private:
		std::map<std::string, GLTexture> textureMap_;
	};

}

#endif