#include "IOManager.h"

#include <fstream>

namespace T3E
{

	bool IOManager::readFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer)
	{
		// reading in binary
		std::ifstream file(filePath, std::ios::binary);
		if (file.fail())
		{
			perror(filePath.c_str());
			return false;
		}
		//seek to the end of the file
		file.seekg(0, std::ios::end);

		// get the file size
		int fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		//reduce the file size by any header bytes that might be present
		fileSize -= file.tellg();

		// resize our buffer
		buffer.resize(fileSize);

		//read our file
		file.read((char *)&buffer[0], fileSize);

		//close the file
		file.close();

		return true;

	}

}