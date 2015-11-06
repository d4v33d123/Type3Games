#pragma once
#include "glTexture.h"
#include <string>
namespace Type3Engine
{
	class ImageLoader
	{
	public:
		static GLTexture loadPNG(std::string filePath);
	};

}