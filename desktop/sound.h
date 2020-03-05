#ifndef SOUND_H
#define SOUND_H

#ifdef _WIN32
	#include <SDL_mixer.h>
#elif _PSP
	#define USE_RWOPS
	#include <SDL/SDL_mixer.h>
#endif

class sound
{
	public:
		sound(){}
		bool load(const char* s);
		bool load_mem(char* buf, int buf_size);
		void play(bool loop);
		bool is_playing();
		~sound();
	private:
		bool playing;
		Mix_Music* music;
};

#endif