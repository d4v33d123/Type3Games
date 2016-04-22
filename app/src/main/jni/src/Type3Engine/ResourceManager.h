#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "TextureCache.h"
#include <string>

namespace T3E
{
	class ResourceManager
	{
	public:
        /**
		*
		* Retrieves texture info from the texture cache
		*
		* @param [in] The string containing the file path to the png texture file
		*
		*/
		static GLTexture getTexture(std::string texturePath);
        
        /**
		*
		* Calls the clear() method of the TextureCache member.
		*
		*/
		static void clearTextures();
	private:
		static TextureCache textureCache_;
	};
}

#endif