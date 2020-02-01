#include "sound.h"

Mix_Music* music = NULL;

bool sound_load(Mix_Music** music, const char* s)
{
	if (*music != NULL){
		Mix_FreeMusic(*music);
		*music = NULL;
	}

	*music = Mix_LoadMUS(s);
	if (*music == NULL)
		return false;

	return true;
}

bool sound_load_mem(Mix_Music** music, char* buf, int buf_size)
{
	if (*music != NULL){
		Mix_FreeMusic(*music);
		*music = NULL;
	}

	SDL_RWops* rw = SDL_RWFromMem((void*)buf, buf_size);
	*music = Mix_LoadMUS_RW(rw);
	if (*music == NULL)
		return false;

	return true;
}

void sound_play(Mix_Music** music, bool* music_playing_flag, bool loop)
{
	if (loop == true)
		Mix_PlayMusic(*music, -1);
	else
		Mix_PlayMusic(*music, 0);
	*music_playing_flag = true;
}

bool sound_isplaying()
{
	return Mix_PlayingMusic() == 0 ? false : true; 
}