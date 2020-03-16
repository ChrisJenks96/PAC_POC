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
static SDL_Color yellow = {255, 255, 0};

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
		TTF_Font* get_main_font(){return mf;}
		void create(char* txt, int x, int y, bool center_text);
		void update();
		void render(SDL_Surface* scr, int n);
		void destroy(int n);
		~font();
	private:
		font_surface* mfs;
		char str[256];
		TTF_Font* mf;
		float ratio;
		int size;

};

#endif