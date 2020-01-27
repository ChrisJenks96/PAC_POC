#ifndef SOUND_H
#define SOUND_H

#ifdef _WIN32
	#include <SDL_mixer.h>
#elif _PSP
	#include <SDL/SDL_mixer.h>
#endif

bool sound_load(Mix_Music** music, const char* s);
void sound_play(Mix_Music** music, bool* music_playing_flag, bool loop);
bool sound_isplaying();

#endif