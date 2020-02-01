#ifndef SOUND_H
#define SOUND_H

#ifdef _WIN32
	#include <SDL_mixer.h>
#elif _PSP
	#define USE_RWOPS
	#include <SDL/SDL_mixer.h>
#endif

extern Mix_Music* music;

bool sound_load(Mix_Music** music, const char* s);
bool sound_load_mem(Mix_Music** music, char* buf, int buf_size);
void sound_play(Mix_Music** music, bool* music_playing_flag, bool loop);
bool sound_isplaying();

#endif