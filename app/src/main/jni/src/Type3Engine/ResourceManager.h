#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "TextureCache.h"
#include <string>

namespace T3E
{
	class ResourceManager
	{
	public:
		static GLTexture getTexture(std::string texturePath);
		static void clearTextures();
	private:
		static TextureCache textureCache_;
	};
}

#endif