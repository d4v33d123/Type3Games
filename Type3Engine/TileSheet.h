#pragma once

#include "glTexture.h"
#include <glm/glm.hpp>

namespace T3E
{
	class TileSheet
	{
	public:
		void init(const GLTexture& texture, const glm::ivec3& tileDims);

		glm::vec4 getUVs(int index);

	private:
		GLTexture texture;
		glm::ivec3 dims;

	};
}

