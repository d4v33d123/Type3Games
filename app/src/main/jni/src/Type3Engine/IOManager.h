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
		/**
		*
		* Reads the Binary from the file to a buffer
		*
		* @param [in] The string containing the file path that is going to be accessed
		* @param [in] The unsigned character buffer that the binary is going to be written into
		* @return true on success
		*
		*/
		static bool readBinaryToBuffer(std::string filePath, std::vector<unsigned char>& buffer);
				
		/**
		*
		* Reads the text from the file to a buffer of chars and appents a '\0' on the end
		*
		* @param [in] The string containing the file path that is going to be accessed
		* @param [in] The character buffer that the text is going to be written into
		* @return true on success
		*
		*/
		static bool readTextToBuffer(std::string filePath, std::vector<char>& buffer);
	};
}

#endif