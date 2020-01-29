#include "video.h"

#ifdef _WIN32
	int done;
	int use_audio, use_video;
	int fullscreen;
	int scalesize;
	int scale_width, scale_height;
	int loop_play;
	int video_index, pause;
	int volume;
	Uint32 seek;
	//float skip;
	int bilinear_filtering;
	SMPEG *mpeg;
	SMPEG_Info info;
	char *basefile;
	SDL_version sdlver;
	SMPEG_version smpegver;
	int fd;
	char buf[32];
	int status;

	bool pc_video_setup(char* fn, SDL_Surface* screen, int scr_w, int scr_h)
	{
		/* Get the command line options */
		use_audio = 1;
		use_video = 1;
		fullscreen = 0;
		scalesize = 0;
		scale_width = 0;
		scale_height = 0;
		loop_play = 0;
		volume = 100;
		seek = 0;
		//skip = 0;
		bilinear_filtering = 0;
		fd = 0;

		//mpeg = SMPEG_new_descr(0, &info, use_audio);
		mpeg = SMPEG_new(fn, &info, use_audio);
		if (SMPEG_error(mpeg)) 
		{
			printf("%s: %s\n", fn, SMPEG_error(mpeg));
			SMPEG_delete(mpeg);
			status = -1;
			return false;
		}

		SMPEG_enableaudio(mpeg, use_audio);
		SMPEG_enablevideo(mpeg, use_video);
		SMPEG_setvolume(mpeg, volume);

		/* Enable software bilinear filtering, if desired */
		if ( bilinear_filtering ) 
		{
			SMPEG_Filter *filter;
			filter = SMPEGfilter_bilinear();
			filter = SMPEG_filter( mpeg, filter );
			filter->destroy(filter);
		}

		/* Set up video display if needed */
		if ( info.has_video && use_video ) 
		{
			const SDL_VideoInfo *video_info;
			int video_bpp;
			int width, height;

			/* Get the "native" video mode */
			video_info = SDL_GetVideoInfo();
			switch (video_info->vfmt->BitsPerPixel) {
				case 16:
					video_bpp = 16;
					break;
				case 24:
					video_bpp = 24;
					break;
				case 32:
					video_bpp = 32;
					break;
				default:
					video_bpp = 16;
					break;
			}
			if (scale_width)
				width = scale_width;
			else
				width = info.width;
			width *= scalesize;

			if ( scale_height )
				height = scale_height;
        
			else
				height = info.height;
			height *= scalesize;

			SMPEG_setdisplay(mpeg, screen, NULL, pc_video_update);
			SMPEG_scaleXY(mpeg, scr_w, scr_h);
		}

		/* Set any special playback parameters */
		if ( loop_play ) {
			SMPEG_loop(mpeg, 1);
		}

		/* Seek starting position */
		if(seek) SMPEG_seek(mpeg, seek);

		/* Skip seconds to starting position */
		//if(skip) SMPEG_skip(mpeg, skip);
		return true;
	}

	void pc_video_play(SDL_Surface* screen)
	{
		/* Play it, and wait for playback to complete */
		SMPEG_play(mpeg);
		done = 0;
		pause = 0;

		while (!done && SMPEG_status(mpeg) == SMPEG_PLAYING)
		{
			SDL_Event event;
			while (use_video && SDL_PollEvent(&event)) 
			{
				switch (event.type) 
				{
					case SDL_QUIT:
						done = 1;
						break;
					default:
						break;
				}
			}

			SDL_Delay(GAME_DEFAULT_DELAY);
		}
	}

	void pc_video_destroy()
	{
		SMPEG_delete(mpeg);
	}

	void pc_video_update(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
	{
		#ifdef _WIN32
			if ( screen->flags & SDL_DOUBLEBUF ) {
				SDL_Flip(screen);
			}
		#elif _PSP
			SDL_Flip(screen);
		#endif
	}

	void pc_next_movie(int sig)
	{
		done = 1;
	}
#endif

	bool video_play(SDL_Surface* scr, char* n)
	{
		/*#ifdef _WIN32
			if (pc_video_setup(n, scr, scr->w, scr->h)){
				pc_video_play(scr);
				pc_video_destroy();
				return true;
			}
			return false;
		#elif _PSP*/
			if (vog_setup("DUMP.vog", "DUMP.ogg", scr->w, scr->h) != -1)
				return true;
			return false;
		//#endif
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
		#ifdef _WIN32
			strcpy(&buff[_strlen(fn)], ".mpg");
			//shutdown the audio mixer first else we cant play
			Mix_CloseAudio();
			video_play(scr, buff);
			//reopen the audio mixer
			Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
		#elif _PSP
			strcpy(&buff[_strlen(fn)], ".pmp");
			//SDL_Quit();
			video_play(scr, buff);
			//we have to close SDL, play the vid, reopen SDL
			//sys_init();
		#endif
	}
}