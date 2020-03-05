#ifndef UTIL_H
#define UTIL_H

#ifdef _WIN32
	#include <SDL.h>
	#include <SDL_ttf.h>
	//performance shite
	#include <windows.h>
#else
	#include <pspctrl.h>
	#include <SDL/SDL.h>
	#include <SDL/SDL_ttf.h>
#endif

static unsigned char cga_palette[] =
{
    0x00, 0x00, 0x00, //black 0
    0x00, 0x00, 0xA8, //blue 1
    0x00, 0xA8, 0x00, //green 2
    0x00, 0xA8, 0xA8, //cyan 3
    0xA8, 0x00, 0x00, //red 4
    0xA8, 0xA8, 0x00, //magenta 5
    0xA8, 0x54, 0x00, //brown 6
    0xA8, 0xA8, 0xA8, //grey 7
    0x54, 0x54, 0x54, //dark grey 8
    0x54, 0x54, 0xFC, //light blue 9
    0x54, 0xFC, 0x54, //light green 10
    0x54, 0xFC, 0xFC, //light cyan 11
    0xFC, 0x54, 0x54, //light red 12
    0xFC, 0x54, 0xFC, //light magenta 13
    0xFC, 0xFC, 0x54, //yellow 14
    0xFC, 0xFC, 0xFC //white 15
};


static SDL_Surface* load_bmp(char* fn)
{
	SDL_Surface* s;
	char buff[32];
	s = SDL_LoadBMP(buff);
	if (!s)
		return NULL;
	else
		return s;
}

static Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

static void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

//useful scaling of a surface, cannot do it in SDL 1.2 natively, so this function stretches the pixels
static SDL_Surface* scale_surface(SDL_Surface *Surface, Uint16 Width, Uint16 Height)
{
	if (Surface->w == Width && Surface->h == Height)
		return Surface;

	else
	{
		if(!Surface || !Width || !Height)
			return 0;
     
		SDL_Surface *_ret = SDL_CreateRGBSurface(Surface->flags, Width, Height, Surface->format->BitsPerPixel,
			Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);
 
		double _stretch_factor_x = (static_cast<double>(Width)  / static_cast<double>(Surface->w)),
			_stretch_factor_y = (static_cast<double>(Height) / static_cast<double>(Surface->h));
 
		for(Sint32 y = 0; y < Surface->h; y++)
		{
			for(Sint32 x = 0; x < Surface->w; x++)
			{
				for(Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
				{
					for(Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
					{
						putpixel(_ret, static_cast<Sint32>(_stretch_factor_x * x) + o_x, 
							static_cast<Sint32>(_stretch_factor_y * y) + o_y, getpixel(Surface, x, y));
					}
				}
			}
		}
 
		SDL_FreeSurface(Surface);
		Surface = NULL;
		return _ret;
	}
}

//find a character in the string
static int str_find(char* s, char c)
{
	int id = 0, k = 0;
	while (true)
	{
		id = s[k] == c ? k : id;
		k++;
		if (s[k] == 0)
			break;
	}

	return id;
}

static unsigned int cga_palette_to_rgba_uint(unsigned char col_id)
{
	unsigned char r, g, b;
	r = cga_palette[col_id*3];
	g = cga_palette[(col_id*3)+1];
	b = cga_palette[(col_id*3)+2];
	return (r << 24) | (g << 16) | (b << 8) | 255;
}

#endif