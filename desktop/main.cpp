#include "core.h"
#include "bin_image.h"

int main(int argc, char** argv)
{	
	core main_core;
	main_core.init(60);
	bin_image_loader b_loader;
	SDL_Surface* player_surface = b_loader.load("PLAYER.bin", 16, 16);

	while (!main_core.is_quit())
	{
		//prelim updates before render everything out
		main_core.pre_update();
		main_core.update();
		//put your sprite rendering code here...
		SDL_BlitSurface(player_surface, 0, main_core.get_screen(), 0);
		//render everything out
		main_core.render();
	}

	SDL_FreeSurface(player_surface);
	
	return 0;
}