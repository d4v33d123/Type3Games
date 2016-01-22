#ifndef ERRORS_H
#define ERRORS_H

#include "SDL.h"
#include <cstdlib>
#include <string>

namespace T3E
{
	extern void fatalError(std::string errorString);
}

#endif