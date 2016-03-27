#include "AudioEngine.h"
#include "Type3Engine.h"

#ifdef __cplusplus 
extern "C" {
#endif

namespace T3E
{
	// play sound effect
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
	// play the music
	void Music::play(int loops)
	{
		Mix_PlayMusic(m_music, loops);
	}
	// pause the music
	void Music::pause()
	{
		Mix_PauseMusic();
	}
	// resume the music
	void Music::resume()
	{
		Mix_ResumeMusic();
	}
	// stop the music
	void Music::stop()
	{
		Mix_HaltMusic();
	}
	// constuctor
	AudioEngine::AudioEngine()
	{
		
	}
	// destructor
	AudioEngine::~AudioEngine()
	{SDL_Log("destruction ***");
		destroy();
	}
	// initialise
	void AudioEngine::init()
	{
		// check to see if MP3 and OGG are working, Could just be OGG since we are only using them
		if(Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == -1)
		{
			fatalError("Mix_Init has encountered an error: " + std::string(Mix_GetError()));
		}
		
		//  open 44.1KHz, signed 16bit, system byte order, stereo audio, using 1024 byte chunks
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
			
 			std::map<std::string, Mix_Chunk*>::iterator e_it;
			std::map<std::string, Mix_Music*>::iterator m_it;		
			
			for ( e_it = m_effectMap.begin(); e_it != m_effectMap.end(); e_it++ )
			{
				Mix_FreeChunk(e_it->second);
				e_it->second = NULL;
			}
			for ( m_it = m_musicMap.begin(); m_it != m_musicMap.end(); m_it++ )
			{
				Mix_FreeMusic(m_it->second);
				m_it->second = NULL;
			}
			
			m_effectMap.clear();
			m_musicMap.clear();
			
			Mix_Quit();
		}
	}
	
	// Loading sound effects
	SoundEffect AudioEngine::loadSoundEffect(const std::string& filePath)
	{
		// check the sound cache
		auto it = m_effectMap.find(filePath);
		
		SoundEffect effect;
		
		if(it == m_effectMap.end())
		{
			// load the sound effect from memory and store it in the cache
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
			// get the sound effect from the memory
			effect.m_chunk = it->second;
		}
		
		return effect;
	}
	
	// load the music
	Music AudioEngine::loadMusic(const std::string& filePath)
	{
		// check the music cache
		auto it = m_musicMap.find(filePath);
		
		Music music;
		
		if(it == m_musicMap.end())
		{
			// load the music from memory and store it in the cache
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
			// get the music from the cache
			music.m_music = it->second;
		}
		
		return music;
	}
	
}

#ifdef __cplusplus
}
#endif