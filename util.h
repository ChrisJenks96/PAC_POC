#ifndef UTIL_H
#define UTIL_H

#ifdef _WIN32
	#include <SDL.h>
	#include <SDL_ttf.h>
#else
	#include <pspctrl.h>
	#include <SDL/SDL.h>
	#include <SDL/SDL_ttf.h>
#endif

//according to https://websemantics.uk/articles/font-size-conversion/
#define FONT_POINT_TO_PX(x) (int)(x * 1.3f)
#define NUM_OF(x) (sizeof(x)/sizeof(x[0]))

typedef struct vec2_int32
{
	int32_t x, y;
} vec2_int32;

#ifdef _PSP
	extern SceCtrlData pad;
#endif

extern char str[256];
extern SDL_Rect debug_r;
extern uint8_t mouse_down_count;
extern int mx, my;
extern SDL_Rect cursor_dest;

extern float timed_bool_toggle_counter; //0 -> toggle_max
extern int timed_bool_toggle_max; //20

//colors for text
static SDL_Color r = {255, 0, 0};
static SDL_Color g = {0, 255, 0};
static SDL_Color b = {0, 0, 255};
static SDL_Color w = {255, 255, 255};

typedef struct font_surface
{
	SDL_Rect r;
	SDL_Surface* s;
} font_surface;

void mouse_update(float dt);
void pc_mouse_debug_coord(float dt);
//timed key press
bool timed_bool(bool* k, bool* k_press, float dt, float s);

//hacky string end for our string format w/ color
int str_end(char* s, int size);

//multi coloured fonts
font_surface* font_multicol_setup(TTF_Font* f, char* txt, SDL_Color b, SDL_Color h, int x, int y, bool center_text);
void font_multicol_render(SDL_Surface* scr, font_surface* s, int n);
void font_multicol_destroy(font_surface* s, int n);

//pixel manipulation
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
SDL_Surface* gradient_background_create();

#endif