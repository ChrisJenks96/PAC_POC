#include "enemy.h"

void enemy::update(core* mc, sprite* player)
{
	//get 2 samples of the cat following the player
    //from them two position, subtract to get directional vector
    //then force the directional vector on the player, static but 
    //means the nasty cat will have a general idea of where the player is
    if (cycle < 2)
    {
		old_dir_x[cycle] = (player->get_sr()->x - sr.x);
        old_dir_y[cycle] = (player->get_sr()->y - sr.y);
        sr.x += old_dir_x[cycle] * -0.01f;
        sr.y += old_dir_y[cycle] * -0.01f;
        cycle++;
    }

    else if (cycle == 2)
    {
        //get the sample data and make our directional vector
        dir_x = old_dir_x[1] - old_dir_x[0];
        dir_y = old_dir_y[1] - old_dir_y[0];
        printf("old_dir_x[1]: %i | old_dir_x[0]: %i\n", old_dir_x[1], old_dir_x[0]);
        printf("old_dir_y[1]: %i | old_dir_y[0]: %i\n", old_dir_y[1], old_dir_y[0]);
        //force it out of these two config statements
        cycle = 3;
    }

    else if (cycle > 2)
    {
        //printf("sr.x: %i | sr.y: %i\n", sr.x, sr.y);
        sr.x += dir_x * 0.01f;
        sr.y += dir_y * 0.01f;
    }

	world_bound_check(mc->get_screen()->w, mc->get_screen()->h);
}

void enemy::reset_pos()
{
	//reset position back, random x, we need the cat out of shot
    sr.x = 150;
    sr.y = 0;
    //cycle 0 resets the sample data for direction
    cycle = 0;
    visible = true;
}

void enemy::world_bound_check(int scr_w, int scr_h)
{
	//bounds checks, if the cat is out, reset it and do it again
    bool out_of_bounds = false;
    if (sr.x > (scr_w - sr.w))
        out_of_bounds = true;
    if (sr.x < 0 + sr.w)
        out_of_bounds = true;
    if (sr.y > (scr_h - sr.h))
        out_of_bounds = true;

    if (out_of_bounds)
        reset_pos();
}