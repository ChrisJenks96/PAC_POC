#include "core.h"
#include "bin_image.h"
#include "physics_sprite.h"
#include "sprite.h"

//constants for storing level data
#include "level.h"

//resetting the treat when it goes out of world bounds
static void treat_reset_check(physics_sprite* treat)
{
	if (treat->get_sr()->y >= SCR_HEIGHT)
    {
        //treat->speedy = -1; //random speed vector for falling
		treat->get_sr()->y = 0; //reset off screen
        treat->get_sr()->x = SCR_WIDTH / 2;//Math.random() * game_area.canvas.width;
        treat->visible = true;
    }
}

static void player_move_update(core* main_core, physics_sprite* player, sprite* platform, physics_sprite* treat, 
							   unsigned int* treat_collects)
{
	int c = 0;
	//zero the velocity of the sprite off
	player->zero();
	//player pressed 'd'
	if (main_core->get_input_keys()[100])
	{
		for (c = 0; c < lvl_0_tile_size; c++){
			player->right_collision(&platform[c]);
		}

		player->speedx = 1;
	}

	//player pressed 'a'
	if (main_core->get_input_keys()[97])
	{
		for (c = 0; c < lvl_0_tile_size; c++){
			player->left_collision(&platform[c]);
		}

		player->speedx = -1;
	}

	//do collision checks against the platforms
	for (c = 0; c < lvl_0_tile_size; c++){
		player->up_collision(&platform[c]);
		player->down_collision(&platform[c]);
	}

	//check to see if the player is in the water or not
	player->in_water_collision_update(SCR_HEIGHT - 50);
	//make sure the player does not escape bounds
	player->out_of_world_update(SCR_HEIGHT, lvl_0_tile_xy);
	//jump routine for the player
	player->jump_update(main_core->get_input_keys()[32]);

	//if we collide with the treat, increment
	if (player->collision_update(treat))
	{
		//sound_get_treat.play();
        //if the treat is visible, add it to our conut
        if (treat->visible)
        {
            *treat_collects+=1;
            treat->visible = false;
            /*//update the platforms if we've collected x number of treats
            platform_update();
            //reset the platform update flag
            platform_update_flag = false;*/
        }
	}

	treat_reset_check(treat);

	//physics sprite update code here....
	player->update(SCR_WIDTH, SCR_HEIGHT);
}

//outputting the remaining health and score to the screen
static void scores_text_output(core* mc, int num_lives, int num_collects)
{
	//create the text as an sdl surface, render it then destroy it 
	//for the next line of text we need
	char buffer[32];
	sprintf(buffer, "<y>X%i<y>", num_lives);
	mc->get_font()->create(buffer, 32, 9, false);
	mc->get_font()->render(mc->get_screen(), 1);
	mc->get_font()->destroy(1);

	sprintf(buffer, "<r>X%i<r>", num_collects);
	mc->get_font()->create(buffer, 292, 9, false);
	mc->get_font()->render(mc->get_screen(), 1);
	mc->get_font()->destroy(1);
}

int main(int argc, char** argv)
{	
	core main_core;
	//run the game at 60 fps
	main_core.init(60);
	//create the binary loader for the images
	bin_image_loader b_loader;
	//load in the resources for the game
	unsigned int treat_collects = 0, lives_left = 3;
	sprite platform[lvl_0_tile_size], treat_icon, heart_icon, water;
	physics_sprite player, nasty_cat, treat;

	water.init(1, SCR_WIDTH, 32, 0, SCR_HEIGHT - 32);
	player.init(b_loader.load("ASSETS/DOG.bin", 16, 16), lvl_0_tile_xy[0] - 32, lvl_0_tile_xy[1] - 96);
	
	int index, c = 0;
	for (index = 0; index < lvl_0_tile_size; index++){
		platform[index].init(b_loader.load("ASSETS/PLATFORM.bin", 16, 16), lvl_0_tile_xy[c] - 32, lvl_0_tile_xy[c+1] - 32);
		c+=2;
	}

	treat.init(b_loader.load("ASSETS/TREAT.bin", 16, 16), 80, 0);
	nasty_cat.init(b_loader.load("ASSETS/NASTY_CAT.bin", 16, 16), 0, 0);
	treat_icon.init(b_loader.load("ASSETS/TREAT.bin", 16, 16), 10, 9);
	heart_icon.init(b_loader.load("ASSETS/HEART.bin", 16, 16), 270, 9);

	while (!main_core.is_quit())
	{
		//prelim updates before render everything out
		main_core.pre_update();
		main_core.update();

		player_move_update(&main_core, &player, platform, &treat, &treat_collects); 

		nasty_cat.update(SCR_WIDTH, SCR_HEIGHT);
		treat.update(SCR_WIDTH, SCR_HEIGHT);

		//put your sprite rendering code here...
		player.render(main_core.get_screen());
		for (index = 0; index < lvl_0_tile_size; index++)
			platform[index].render(main_core.get_screen());
		treat.render(main_core.get_screen());
		nasty_cat.render(main_core.get_screen());
		treat_icon.render(main_core.get_screen());
		heart_icon.render(main_core.get_screen());
		water.render(main_core.get_screen());
		//render out the scores and lives
		scores_text_output(&main_core, treat_collects, lives_left);
		//render everything out
		main_core.render();
	}

	return 0;
}