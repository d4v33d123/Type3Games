#include "errors.h"

#include <SDL/SDL.h>
#include <iostream>
#include <cstdlib>
namespace T3E
{

	void fatalError(std::string errorString)
	{
		std::cout << errorString << std::endl;
		std::cout << "Enter any key to quit...";
		int tmp;
		std::cin >> tmp;
		SDL_Quit();
		exit(1);
	}
}