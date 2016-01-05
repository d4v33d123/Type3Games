#ifndef ERRORS_H
#define ERRORS_H

#include "SDL.h"
#include <cstdlib>
#include <string>

namespace Type3Engine
{
	extern void fatalError(std::string errorString);
}

#endif