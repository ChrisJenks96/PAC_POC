#ifndef CORE_H
#define CORE_H

#ifdef _WIN32
	#include <SDL.h>
	#include <SDL_ttf.h>
	#include <SDL_mixer.h>

	#define SCR_WIDTH 320
	#define SCR_HEIGHT 200
	#define TEX_WIDTH 320
	#define TEX_HEIGHT 200
	#define SCR_BPP 32
	#define SCR_SURF_MODE SDL_HWSURFACE// | SDL_FULLSCREEN
#else
	#include <pspkernel.h>
	#include <pspgu.h>
	#include <pspdebug.h>
	#include <pspdisplay.h>
	#include <pspctrl.h>
	#include "callbacks.h"
	#include <SDL/SDL.h>
	#include <SDL/SDL_ttf.h>
	#include <SDL/SDL_mixer.h>

	#define SCR_WIDTH 320
	#define SCR_HEIGHT 200
	#define TEX_WIDTH 320
	#define TEX_HEIGHT 200
	#define SCR_BPP 32
	#define SCR_SURF_MODE SDL_SWSURFACE

	#define printf pspDebugScreenPrintf

	PSP_MODULE_INFO("Who's a Good Boy!", 0, 1, 0);
	PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
	PSP_HEAP_SIZE_MAX();
#endif

#include "font.h"
#include "input.h"

class core
{
	public:
		core(){};
		bool init(int _fps);
		void pre_update();
		void update();
		void render();
		bool* get_input_keys(){return in.get_keys();}
		SDL_Surface* get_screen(){return screen;}
		float get_deltatime(){return deltatime;}
		bool is_quit(){return in.is_quit();}
		~core();
	private:
		int fps;
		float deltatime;
		font main_font;
		input in;
		SDL_Surface* screen;
		int start_clock;
};

#endif