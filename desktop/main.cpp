#include "core.h"
#include "bin_image.h"
#include "sprite.h"

//constants for storing level data
#include "level.h"

int main(int argc, char** argv)
{	
	core main_core;
	//run the game at 60 fps
	main_core.init(60);
	//create the binary loader for the images
	bin_image_loader b_loader;
	//load in the resources for the game
	sprite player, platform[lvl_0_tile_size], treat, nasty_cat, treat_icon, heart_icon, water;

	water.init(1, SCR_WIDTH, 32, 0, SCR_HEIGHT - 32);
	player.init(b_loader.load("DOG.bin", 16, 16), lvl_0_tile_xy[0] - 32, lvl_0_tile_xy[1] - 96);
	
	int index, c = 0;
	for (index = 0; index < lvl_0_tile_size; index++){
		platform[index].init(b_loader.load("PLATFORM.bin", 16, 16), lvl_0_tile_xy[c] - 32, lvl_0_tile_xy[c+1] - 32);
		c+=2;
	}

	treat.init(b_loader.load("TREAT.bin", 16, 16), 80, 0);
	nasty_cat.init(b_loader.load("NASTY_CAT.bin", 16, 16), 0, 0);
	treat_icon.init(b_loader.load("TREAT.bin", 16, 16), 10, 9);
	heart_icon.init(b_loader.load("HEART.bin", 16, 16), 270, 9);

	while (!main_core.is_quit())
	{
		//prelim updates before render everything out
		main_core.pre_update();
		main_core.update();
		//put your sprite rendering code here...
		player.render(main_core.get_screen());
		for (index = 0; index < lvl_0_tile_size; index++)
			platform[index].render(main_core.get_screen());
		treat.render(main_core.get_screen());
		nasty_cat.render(main_core.get_screen());
		treat_icon.render(main_core.get_screen());
		heart_icon.render(main_core.get_screen());
		water.render(main_core.get_screen());
		//render everything out
		main_core.render();
	}

	return 0;
}