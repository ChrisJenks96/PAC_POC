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
		sprite(){visible = true;}
		SDL_Rect* get_sr(){return &sr;}
		bool init(SDL_Surface* _s, short x, short y);
		bool init(unsigned char rgb_index, short w, short h, short _x, short _y);
		void render(SDL_Surface* scr);
		~sprite();

		bool visible;
	protected:
		SDL_Surface* s;
		SDL_Rect sr;
};

#endif