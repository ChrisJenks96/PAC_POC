#include "sound.h"

bool sound::load(const char* s)
{
	/*if (music != NULL){
		Mix_FreeMusic(music);
		music = NULL;
	}

	music = Mix_LoadMUS(s);
	if (music == NULL)
		return false;*/

	return true;
}

bool sound::load_mem(char* buf, int buf_size)
{
	/*if (music != NULL){
		Mix_FreeMusic(music);
		music = NULL;
	}

	SDL_RWops* rw = SDL_RWFromMem((void*)buf, buf_size);
	music = Mix_LoadMUS_RW(rw);
	if (music == NULL)
		return false;*/

	return true;
}

void sound::play(bool loop)
{
	/*if (loop == true)
		Mix_PlayMusic(music, -1);
	else
		Mix_PlayMusic(music, 0);*/
	playing = true;
}

bool sound_isplaying()
{
	return true;//Mix_PlayingMusic() == 0 ? false : true; 
}

sound::~sound()
{
	/*if (music != NULL){
		Mix_FreeMusic(music);
		music = NULL;
	}*/
}