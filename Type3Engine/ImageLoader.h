#pragma once
#include "glTexture.h"
#include <string>
namespace T3E
{
	class ImageLoader
	{
	public:
		static GLTexture loadPNG(std::string filePath);
	};

}