#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#ifdef _WIN32
	#include <SDL.h>
	#include <SDL_ttf.h>
#else
	#include <SDL/SDL.h>
	#include <SDL/SDL_ttf.h>
#endif

#include "util.h"

extern int font_select_id;
extern SDL_Color font_c_highlight;
extern SDL_Color font_c_unhighlight;
extern SDL_Rect r1;
extern SDL_Surface* s;

extern SDL_Surface* grad_bkg;

void main_menu_setup();
bool mm_font_setup_surface(TTF_Font* f, SDL_Surface* scr, int font_size, char* txt, int y_offset);
void main_menu_render(TTF_Font* f, SDL_Surface* scr, int font_size);
void main_menu_destroy();

#endif
