#ifndef SPRITE_H
#define SPRITE_H

#ifdef _WIN32
	#include <SDL.h>
#else
	#include <SDL/SDL.h>
#endif

#include "util.h"

class sprite
{
	public:
		sprite(){}
		bool init(SDL_Surface* _s, unsigned short x, unsigned short y);
		bool init(unsigned char rgb_index, unsigned short w, unsigned short h, unsigned short _x, unsigned short _y);
		void render(SDL_Surface* scr);
		~sprite();
	private:
		SDL_Surface* s;
		SDL_Rect sr;
};

#endif