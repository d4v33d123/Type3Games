#ifndef ERRORS_H
#define ERRORS_H

#include "SDL.h"
#include <cstdlib>
#include <string>

namespace T3E
{
	/**
	* 
	* Kills the program if a fatal error occurs
	* 
	* @param [in] The error message to be printed
	*
	*/
	extern void fatalError(std::string errorString);
}

#endif