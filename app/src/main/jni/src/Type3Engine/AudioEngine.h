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
		
		/**
		* Plays the sound effect and does not repeat
		*
		* the int loops is set already, there is no need to pass a variable into the function 
		*/
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
		
		/**
		* Plays the music and is set to repeat
		*
		* the int loops is set already, there is no need to pass a variable into the function 
		*/
		void play(int loops = 1);
		
		/**
		* Pauses the music
		*/
		// pause the music
		static void pause();
		
		/**
		* Stops the music
		*/
		// stops the music
		static void stop();
		
		/**
		* Resumes the music
		*/
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
		
		/**
		* initialises the audio engine
		*/
		void init();
		
		/**
		* Safely destroys the audio engine
		*/
		void destroy();
		
		/**
		* Loads a sound effect from the file path
		*
		* @param [in] The string contatining the file path
		* @return [out] The Sound effect that is created from this file
		*/
		SoundEffect loadSoundEffect(const std::string& filePath);
		
		/**
		* Loads music from the file path
		*
		* @param [in] The string contatining the file path
		* @return [out] The Music that is created from this file
		*/
		Music loadMusic(const std::string& filePath);
		
		/**
		* Clears the Maps containing all the music and sound effects
		*/
		void clearMaps();//TESTING
		
	private:
	
		std::map<std::string, Mix_Chunk*>m_effectMap;
		std::map<std::string, Mix_Music*>m_musicMap;
		
		bool m_isInitialized = false;
	};
}

#endif