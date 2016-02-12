#ifndef TEXTURECACHE_H
#define TEXTURECACHE_H

#include <map>
#include "glTexture.h"
#include "ImageLoader.h"
#include <iostream>

namespace T3E
{
	class TextureCache
	{
	public:
		TextureCache();
		~TextureCache();

		GLTexture getTexture(std::string texturePath);

	private:
		std::map<std::string, GLTexture> textureMap_;
	};

}

#endif