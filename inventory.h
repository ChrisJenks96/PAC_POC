#ifndef INVENTORY_H
#define INVENTORY_H

#ifdef _WIN32
	#include <SDL.h>
#else
	#include <SDL/SDL.h>
#endif

#include "util.h"

#define INV_TILE_COUNT 4
#define INV_TILE_SIZE 64
#define INV_TILE_SEPERATION 10

extern bool inv_key_press;
extern bool inventory_grey_scale_cycle;
extern bool is_inventory;
extern SDL_Rect r_inv_source[INV_TILE_COUNT];
extern SDL_Rect r_inv_dest[INV_TILE_COUNT];
extern SDL_Surface* inv;

void inventory_init(char* fn);
void inventory_destroy();
void inventory_screen_fade(SDL_Surface* scr);
void inventory_render(SDL_Surface* scr);


#endif