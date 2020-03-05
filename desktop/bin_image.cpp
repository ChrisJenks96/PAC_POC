#include "bin_image.h"

SDL_Surface* bin_image_loader::load(char* fn, int width, int height)
{
	FILE* f = fopen(fn, "rb");
	if (!f)
		return NULL;
	else
	{
		int y, x, c = 0;
		fseek(f, 0, SEEK_END);
		int size = ftell(f);
		fseek(f, 0, SEEK_SET);
		//load the pixel data into the buffer
		char* buffer = new char[size];
		fread(&buffer[0], size, 1, f);
		SDL_Surface* s = SDL_CreateRGBSurface(0, width, height, 
			32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
		unsigned int rgba_col;
		unsigned char first_col, second_col;
		for (y = 0; y < height; y++){
			for (x = 0; x < width; x+=2){
				//XXXX YYYY -> 0000 XXXX
				first_col = buffer[c] >> 4;
				rgba_col = cga_palette_to_rgba_uint(first_col);
				putpixel(s, x, y, rgba_col);
				//XXXX YYYY -> 0000 YYYY
				second_col = buffer[c] & 0x0F;
				rgba_col = cga_palette_to_rgba_uint(second_col);
				putpixel(s, x+1, y, rgba_col);
				c++;
			}
		}

		delete buffer;
		fclose(f);
		return scale_surface(s, 128, 128);
	}
}

bin_image_loader::~bin_image_loader()
{

}