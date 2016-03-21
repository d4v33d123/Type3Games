#include "MainGame.h"
#include <time.h>

int main(int argc, char** argv)
{
	srand(time(NULL));
	
	MainGame main_Game;

	main_Game.run();

	return 0;
}