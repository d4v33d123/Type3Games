#include "ResourceManager.h"

namespace T3E
{
	TextureCache ResourceManager::textureCache_;

	GLTexture ResourceManager::getTexture(std::string texturePath)
	{
		return textureCache_.getTexture(texturePath);
	}
}