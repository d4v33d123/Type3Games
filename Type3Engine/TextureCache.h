#pragma once
#include <map>
#include "glTexture.h"
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