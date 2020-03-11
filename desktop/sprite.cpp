#include "sprite.h"

bool sprite::init(SDL_Surface* _s, unsigned short _x, unsigned short _y)
{
	s = _s;
	sr.x = _x;
	sr.y = _y;
	sr.w = _s->w;
	sr.h = _s->h;
	return true;
}

bool sprite::init(unsigned char rgb_index, unsigned short w, unsigned short h, unsigned short _x, unsigned short _y)
{
	//*=3 because we pass an index in
	rgb_index *= 3;
	unsigned short x, y;
	s = SDL_CreateRGBSurface(0, w, h, 
		32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	//go through x and y and assign the indexed colour from the cga palette to each pixel
	for (y = 0; y < h; y++){
		for (x = 0; x < w; x++){
			unsigned int rgba_col = cga_palette_to_rgba_uint(rgb_index); 
			putpixel(s, x, y, rgba_col);
		}
	}

	sr.x = _x;
	sr.y = _y;
	sr.w = s->w;
	sr.h = s->h;
	return true;
}

void sprite::render(SDL_Surface* scr)
{
	SDL_BlitSurface(s, 0, scr, &sr);
}

//unload the sprite surface when the object is destroyed from stack
sprite::~sprite()
{
	SDL_FreeSurface(s);
}