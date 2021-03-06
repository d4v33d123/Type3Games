#include "TextureCache.h"
#include "ImageLoader.h"
#include <iostream>

namespace T3E
{

	TextureCache::TextureCache()
	{
	}


	TextureCache::~TextureCache()
	{
	}


	GLTexture TextureCache::getTexture(std::string texturePath)
	{
		// using auto so no need for this declaration std::map<std::string, GLTexture>::iterator
		// look up the texture and see if it's in the map
		auto mit = textureMap_.find(texturePath);

		//check if it's not in the map
		if (mit == textureMap_.end())
		{
			// load the texture
			GLTexture newTexture = ImageLoader::loadPNG(texturePath);

			// can use this to flesh out the code a bit, easier to under stand std::pair<std::string, GLTexture> newPair(texturePath, newTexture);
			//textureMap_.insert(newPair);
			// insert it into the map
			textureMap_.insert(make_pair(texturePath, newTexture));

			std::cout << "Loaded Texture\n";

			return newTexture;
		}
		std::cout << "Used Cached Texture!\n";
		// return our texture if it is already in the map
		return mit->second;


	}
}