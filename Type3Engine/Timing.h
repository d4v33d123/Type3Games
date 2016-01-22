
#pragma once 
namespace T3E
{
	class FpsLimiter
	{
	public:
		FpsLimiter();
		void init(float maxFPS);
		
		void setMaxFPS(float maxFPS)
		{
			maxFPS_ = maxFPS;
		}

		void begin();

		// returns current FPS
		float end();

	private:
		void CalculateFPS();

		float fps_;
		float frameTime_;
		float maxFPS_;
		unsigned int startTicks_;

	};
}