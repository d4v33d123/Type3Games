#include "AudioEngine.h"
#include "Type3Engine.h"

#ifdef __cplusplus 
extern "C" {
#endif

namespace T3E
{
	void SoundEffect::play(int loops)
	{
		if(Mix_PlayChannel(-1, m_chunk, loops) == -1)
		{
			if(Mix_PlayChannel(0, m_chunk, loops) == -1)
			{
				fatalError("Mix_PlayChannel has encountered an error: " + std::string(Mix_GetError()));
			}
		}
	}
	
	void Music::play(int loops)
	{
		Mix_PlayMusic(m_music, loops);
	}
	
	void Music::pause()
	{
		Mix_PauseMusic();
	}
	
	void Music::resume()
	{
		Mix_ResumeMusic();
	}
	
	void Music::stop()
	{
		Mix_HaltMusic();
	}
	
	AudioEngine::AudioEngine()
	{
		
	}
	
	AudioEngine::~AudioEngine()
	{
		destroy();
	}
	
	void AudioEngine::init()
	{
		if(Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == -1)
		{
			fatalError("Mix_Init has encountered an error: " + std::string(Mix_GetError()));
		}
		
		if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
		{
			fatalError("Mix_OpenAudio has encountered an error: " + std::string(Mix_GetError()));
		}
		
		m_isInitialized = true;
	}
	
	void AudioEngine::destroy()
	{
		if(m_isInitialized)
		{
			m_isInitialized = false;
			Mix_Quit();
		}
	}
	
	SoundEffect AudioEngine::loadSoundEffect(const std::string& filePath)
	{
		auto it = m_effectMap.find(filePath);
		
		SoundEffect effect;
		
		if(it == m_effectMap.end())
		{
			Mix_Chunk* chunk = Mix_LoadWAV(filePath.c_str());
			
			if(chunk == nullptr)
			{
				fatalError("Mix_LoadWAV has encountered an error: " + std::string(Mix_GetError()));
			}
			
			effect.m_chunk = chunk;
			m_effectMap[filePath] = chunk;	
		}
		else
		{
			effect.m_chunk = it->second;
		}
		
		return effect;
	}
	
	Music AudioEngine::loadMusic(const std::string& filePath)
	{
		auto it = m_musicMap.find(filePath);
		
		Music music;
		
		if(it == m_musicMap.end())
		{
			Mix_Music* mixMusic = Mix_LoadMUS(filePath.c_str());
			
			if(mixMusic == nullptr)
			{
				fatalError("Mix_LoadMUS has encountered an error: " + std::string(Mix_GetError()));
			}
			
			music.m_music = mixMusic;
			m_musicMap[filePath] = mixMusic;
		}
		else
		{
			music.m_music = it->second;
		}
		
		return music;
	}
	
}

#ifdef __cplusplus
}
#endif