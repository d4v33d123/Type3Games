#pragma once
#include <vector>
namespace T3E
{

	// no constructor or destructor, because it's a static class
	class IOManager
	{
	public:
		static bool readFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer);
	};

}