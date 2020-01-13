#include "inventory.h"

bool inv_key_press = false;
bool inventory_grey_scale_cycle = false;
bool is_inventory = false;
SDL_Rect r_inv_source[INV_TILE_COUNT];
SDL_Rect r_inv_dest[INV_TILE_COUNT];
SDL_Surface* inv;

void inventory_init(char* fn)
{
	int i;
	inv = SDL_LoadBMP("inventory.bmp");
	for (i = 0; i < INV_TILE_COUNT; i++)
	{
		r_inv_source[i].w = r_inv_dest[i].w = INV_TILE_SIZE;
		r_inv_source[i].h = r_inv_dest[i].h = INV_TILE_SIZE;
		r_inv_source[i].x = i * INV_TILE_SIZE;
		r_inv_source[i].y = 0;
		r_inv_dest[i].x = INV_TILE_SEPERATION + (i * INV_TILE_SIZE);
		r_inv_dest[i].y = 10;
	}
}

void inventory_screen_fade(SDL_Surface* scr)
{
	if (!inventory_grey_scale_cycle)
	{
		int y, x;
		for (y = 0; y < scr->h; y++)
		{
			for (x = 0; x < scr->w; x++)
			{
				#ifdef _WIN32 //24 bit depth
					Uint8 r, g, b;
					Uint32 ptr = getpixel(scr, x, y);				
					SDL_GetRGB(ptr, scr->format, &r, &g, &b);
					Uint32 gray = (r + g + b) / 3;
					putpixel(scr, x, y, 0xFF000000 | (gray << 16) | (gray << 8) | gray);
				#else //psp 16 bit depth
					//PutPixel16_nolock(scr, x, y, 0xFFFFFFFF);
				#endif
			}
		}

		inventory_grey_scale_cycle = true;
	}
}

void inventory_render(SDL_Surface* scr)
{
	int i;
	if (is_inventory)
	{
		inventory_screen_fade(scr);
		//show the inventory
		for (i = 0; i < 4; i++)
			SDL_BlitSurface(inv, &r_inv_source[i], scr, &r_inv_dest[i]);
	}
}

void inventory_destroy()
{
	SDL_FreeSurface(inv);
}