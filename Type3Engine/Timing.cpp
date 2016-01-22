#include "Timing.h"
#include <SDL/SDL.h>

namespace T3E
{


	FpsLimiter::FpsLimiter()
	{

	}

	void FpsLimiter::init(float maxFPS)
	{
		setMaxFPS(maxFPS);
	}


	void FpsLimiter::begin()
	{
		startTicks_ = SDL_GetTicks();
	}

	// returns current FPS
	float FpsLimiter::end()
	{
		CalculateFPS();

		float frameTicks = SDL_GetTicks() - startTicks_;
		//Limit the FPS to the max FPS
		if (1000.0f / maxFPS_ > frameTicks)
		{
			SDL_Delay(1000.0f / maxFPS_ - frameTicks);
		}

		return fps_;
	}
	
	void FpsLimiter::CalculateFPS()
	{
		static const int NUM_SAMPLES = 1000;
		static float frameTimes[NUM_SAMPLES];
		static int currentFrame = 0;


		static float prevTicks = SDL_GetTicks();

		float currentTicks;
		currentTicks = SDL_GetTicks();

		frameTime_ = currentTicks - prevTicks;
		frameTimes[currentFrame % NUM_SAMPLES] = frameTime_;

		// set previous ticks to new ticks now
		prevTicks = currentTicks;

		currentFrame++;

		int count;

		if (currentFrame < NUM_SAMPLES)
		{
			count = currentFrame;
		}
		else
		{
			count = NUM_SAMPLES;
		}

		float frameTimeAverage = 0;
		for (int i = 0; i < count; i++)
		{
			frameTimeAverage += frameTimes[i];
		}
		frameTimeAverage /= count;

		if (frameTimeAverage > 0)
		{
			fps_ = 1000.0 / frameTimeAverage;
		}
		else
		{
			fps_ = 60.0f;
		}



	}

}