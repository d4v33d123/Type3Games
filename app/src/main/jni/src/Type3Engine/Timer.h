#ifndef TIMER_H
#define TIMER_H

#include "SDL.h"

namespace T3E
{
	class Timer
	{
	public:
		Timer() : end_time_(0) {}
		~Timer() {}

		/**
		* @param [in] milliseconds The time in wiche the timer should be complete
		*/
		void set( Uint32 timeFromNow )
		{
			end_time_ = SDL_GetTicks() + timeFromNow;
		}

		/**
		* @return Positive values in milliseconds untill the timer is complete
		* Zero when the timer is complete
		* Negative values are milliseconds since the timer finished
		*/
		int timeTillDone()
		{
			return end_time_ - SDL_GetTicks();
		}

		/**
		* Returns true then the set time has elapsed
		*/
		bool done()
		{
			return SDL_GetTicks() >= end_time_;
		}

	private:
		Uint32 end_time_;
	};

}

#endif