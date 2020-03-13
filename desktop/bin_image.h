#ifndef BIN_IMAGE_H
#define BIN_IMAGE_H

#ifdef _WIN32 
	#include <SDL.h>
#elif __linux__
	#include <SDL/SDL.h>
#else
	#include <SDL/SDL.h>
#endif

#include "util.h"

class bin_image_loader
{
	public:
		bin_image_loader(){}
		SDL_Surface* load(char* fn, int width, int height);
		~bin_image_loader();
	private:
};

#endif