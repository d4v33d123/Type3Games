#include "IOManager.h"

namespace Type3Engine
{
	bool IOManager::readBinaryToBuffer(std::string filePath, std::vector<unsigned char>& buffer)
	{
		//read file in binary (rb)
		SDL_RWops* file;
		if((file = SDL_RWFromFile(filePath.c_str(), "rb")) == NULL)
		{
			SDL_Log("TYPE 3 ENGINE couldn't open %s", filePath.c_str());
			return false;
		}
		//resize buffer to appropriate size
		Sint64 fileSize;
		fileSize = SDL_RWsize(file);
		buffer.resize(fileSize);	
		//read the file into the buffer
		SDL_RWread(file, &buffer[0], 1, fileSize);//read 1 byte at a time, up to fileSize bytes		
		//close file
		SDL_RWclose(file);
		return true;
	}
	
	bool IOManager::readTextToBuffer(std::string filePath, std::vector<char>& buffer)
	{
		//read file (r)
		SDL_RWops* file;
		if((file = SDL_RWFromFile(filePath.c_str(), "r")) == NULL)
		{
			SDL_Log("TYPE 3 ENGINE couldn't open %s", filePath.c_str());
			return false;
		}
		//resize buffer to appropriate size + 1 to be able to add '\0' at the end
		Sint64 fileSize;
		fileSize = SDL_RWsize(file);
		buffer.resize(fileSize + 1);
		//read the file into the buffer
		SDL_RWread(file, &buffer[0], 1, fileSize);//read 1 byte at a time, up to fileSize bytes		
		//close file
		SDL_RWclose(file);
		//add terminator at end of string
		buffer[fileSize] = '\0';
		return true;
	}
}