#include "video.h"

bool video_play(SDL_Surface* scr, char* n)
{
	if (vog_setup(n, scr->w, scr->h) != -1)
		return true;
	return false;
}

//USE THIS ONE!!!
//parent function to the original function
//houses all platforms
void video_play2(SDL_Surface* scr, char* fn, void (*sys_init)())
{
	if (!skip_cutscenes)
	{
		char buff[32];
		strcpy(&buff[0], fn);
		strcpy(&buff[_strlen(fn)], ".vog");
		video_play(scr, buff);
	}
}