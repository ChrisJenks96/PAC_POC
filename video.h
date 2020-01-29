#ifndef VIDEO_H
#define VIDEO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
	#include <SDL.h>
	#include <SDL_mixer.h>
	#include <smpeg.h>
#elif _PSP
	#include <pspsdk.h>
	#include <pspkernel.h>
	#include <pspgu.h>
	#include <SDL/SDL.h>
	#include <SDL/SDL_mixer.h>
#endif

#include "vog.h"
#include "util.h"

#ifdef _WIN32
	/* Flag telling the UI that the movie or song should be skipped */
	extern int done;
	extern int use_audio, use_video;
	extern int fullscreen;
	extern int scalesize;
	extern int scale_width, scale_height;
	extern int loop_play;
	extern int video_index, pause;
	extern int volume;
	extern Uint32 seek;
	extern float skip;
	extern int bilinear_filtering;
	extern SMPEG *mpeg;
	extern SMPEG_Info info;
	extern char *basefile;
	extern SDL_version sdlver;
	extern SMPEG_version smpegver;
	extern int fd;
	extern char buf[32];
	extern int status;

	bool pc_video_setup(char* fn, SDL_Surface* screen, int scr_w, int scr_h);
	void pc_video_play(SDL_Surface* screen);
	void pc_video_destroy();
	void pc_video_update(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h);
	void pc_next_movie(int sig);
#endif

	bool video_play(SDL_Surface* scr, char* n);
	void video_play2(SDL_Surface* scr, char* fn, void (*sys_init)());

#endif