#ifndef IOMANAGER_H
#define IOMANAGER_H

#include "SDL.h"
#include <vector>
#include <string>

namespace T3E
{
	// no constructor or destructor, because it's a static class
	class IOManager
	{
	public:		
		//read a file in binary into a vector, ret true if success
		
		/**
		*
		* Reads the Binary from the file to a buffer
		*
		* @param [in] The string containing the file path that is going to be accessed
		* @param [in] The unsigned character buffer that the binary is going to be written into
		*
		*/
		static bool readBinaryToBuffer(std::string filePath, std::vector<unsigned char>& buffer);
		
		
		//read text file into a vector of chars and add '\0' at the end, ret true if success
		
		/**
		*
		* Reads the text from the file to a buffer
		*
		* @param [in] The string containing the file path that is going to be accessed
		* @param [in] The character buffer that the text is going to be written into
		*
		*/
		static bool readTextToBuffer(std::string filePath, std::vector<char>& buffer);
	};
}

#endif