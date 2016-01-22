#pragma once
#include "TextureCache.h"
#include <string>
namespace T3E
{
	class ResourceManager
	{
	public:
		static GLTexture getTexture(std::string texturePath);

	private:
		static TextureCache textureCache_;
	};

}