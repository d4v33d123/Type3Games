#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H


#include "SDL_mixer.h"
#include <string>
#include <map>
#include "errors.h"

namespace T3E
{
	class SoundEffect
	{
	public:
		friend class AudioEngine;
		
		SoundEffect(){;};
		~SoundEffect(){m_chunk = nullptr;};
		
		void play(int loops = 0);
	private:
		Mix_Chunk* m_chunk = nullptr;
		
	};
	
	class Music
	{
	public:
		friend class AudioEngine;
		
		Music(){;};
		~Music(){m_music = nullptr;};
		
		void play(int loops = 1);
		
		// pause the music
		static void pause();
		// stops the music
		static void stop();
		// Resumes the music
		static void resume();
		
	private:
		Mix_Music* m_music = nullptr;
	
	};
	
	class AudioEngine
	{
	public:
		AudioEngine();
		~AudioEngine();
		
		void init();
		void destroy();
		
		SoundEffect loadSoundEffect(const std::string& filePath);
		Music loadMusic(const std::string& filePath);
		
		void clearMaps();//TESTING
		
	private:
	
		std::map<std::string, Mix_Chunk*>m_effectMap;
		std::map<std::string, Mix_Music*>m_musicMap;
		
		bool m_isInitialized = false;
	};
}

#endif