#ifndef VIDEO_H
#define VIDEO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
	#include <SDL.h>
	#include <SDL_mixer.h>
#elif _PSP
	#include <pspsdk.h>
	#include <pspkernel.h>
	#include <pspgu.h>
	#include <SDL/SDL.h>
	#include <SDL/SDL_mixer.h>
#endif

#include "vog.h"
#include "util.h"

bool video_play(SDL_Surface* scr, char* n);
void video_play2(SDL_Surface* scr, char* fn, void (*sys_init)());

#endif