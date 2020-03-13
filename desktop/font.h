#ifndef FONT_H
#define FONT_H

#ifdef _WIN32
	#include <SDL_ttf.h>
#elif __linux__
	#include <SDL/SDL_ttf.h>
#else
	#include <SDL/SDL_ttf.h>
#endif

#include "util.h"

//colors for text
static SDL_Color red = {255, 0, 0};
static SDL_Color green = {0, 255, 0};
static SDL_Color blue = {0, 0, 255};
static SDL_Color white = {255, 255, 255};
static SDL_Color gray = {230, 230, 230};

typedef struct font_surface
{
	SDL_Rect r;
	SDL_Surface* s;
} font_surface;

class font
{
	public:
		font(){}
		bool init(int width, int height);
		font_surface* create(TTF_Font* f, char* txt, SDL_Color b, SDL_Color h, int x, int y, bool center_text);
		void update();
		void render(SDL_Surface* scr, font_surface* s, int n);
		void destroy(font_surface* s, int n);
		~font();
	private:
		char str[256];
		TTF_Font* mf;
		float ratio;
		int size;

};

#endif