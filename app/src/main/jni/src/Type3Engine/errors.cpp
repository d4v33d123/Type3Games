#include "errors.h"

namespace T3E
{
	void fatalError(std::string errorString)
	{
		SDL_Log("TYPE 3 ENGINE FATAL ERROR: %s\n", errorString.c_str());
		// std::cout << errorString << std::endl;
		// std::cout << "Enter any key to quit...";
		// int tmp;
		// std::cin >> tmp;
		SDL_Quit();
		exit(1);
	}
}