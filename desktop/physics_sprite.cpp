#ifndef PHYSICS_SPRITE_H
#define PHYSICS_SPRITE_H

#include "physics_sprite.h"

bool physics_sprite::collision_update(physics_sprite* other)
{
	short left = sr.x;
	short right = sr.x + sr.w;
	short top = sr.y;
	short bottom = sr.y + sr.h;
	short other_left = other->get_sr()->x;
	short other_right = other->get_sr()->x + other->get_sr()->w;
	short other_top = other->get_sr()->y;
	short other_bottom = other->get_sr()->y + other->get_sr()->h;
	bool collision = true;
	if ((bottom < other_top) || (top > other_bottom) ||
        (right < other_left) || (left > other_right)) {
            collision = false;
    }

	return collision;
}

//make sure selected sprites don't exceeds the world limits
//no bottom y check as we want sprites to fall through the world (respawn methods)
void physics_sprite::world_bound_check(short scr_w, short scr_h)
{
	if (wb_check)
	{
		if (sr.x > (scr_w - sr.w))
			sr.x = scr_w - sr.w;
		if (sr.x < 0)
			sr.x = 0;
		if (sr.y < 0)
			sr.y = 0;
	}
}

//zero off any velocity on the sprite
void physics_sprite::zero()
{
	speedx = 0;
	speedy = 0;
}

void physics_sprite::in_water_collision_update(short y)
{
	if (sr.y <= y)
		in_water = false;
	else if (sr.y > y)
	{
		//if (!death_sound_once_flag){
          //  sound_death.play();
            //death_sound_once_flag = true;
        //}
        in_water = true;
	}
}

void physics_sprite::out_of_world_update(short scr_h, const unsigned short* lvl_data)
{
	if (sr.y > (scr_h + sr.h)){
        //if (hearts_left > 0)
          //  hearts_left-=1;
        sr.x = lvl_data[0] - 32;
        sr.y = lvl_data[1] - 96;
        //sound_player_respawn.play();
        //death_sound_once_flag = false;
    }
}

void physics_sprite::update(short scr_w, short scr_h)
{
	//if we're falling with gravity, fall normally unless we hit water
	if (gravity_use)
	{
		if (!in_water)
			speedy = GRAVITY_VALUE;
		else
			speedy = GRAVITY_WATER_VALUE;
	}

	sr.x += speedx;
	sr.y += speedy;

	world_bound_check(scr_w, scr_h);
}

void physics_sprite::jump_update(bool jump_key_press)
{
	//this flag disables if we are falling which also disables double jumping which is good.
    can_jump = true;
    //log the current y
    //we are falling
	if (sr.y > old_y)
        can_jump = false;
	//sync the y back up
    old_y = sr.y;

	//space key to jump, set trigger for jumping
	if (jump_key_press && !is_jump && can_jump)
    {
        //log the y for the jumping
		old_jump_y = sr.y;
		is_jump = true;
        gravity_use = false;
        //sound_jump.play();
    }

	if (is_jump)
    {
        //if we hit the top boundary without bringing gravity back, we're stuck at pixel 0 
        //because it manipulates y and not speedy
		if (sr.y <= 0){
            gravity_use = true;
			is_jump = false;
        }

        //move the player up till the maximum jump height is achieved, then reintroduce gravity and make us fall.
		if (!in_water)
            speedy = -1;
        else
            speedy = -1;//-0.35;

		if (sr.y < (old_jump_y - MAX_JUMP_HEIGHT)) {
            speedy = 0;
			is_jump = false;
            gravity_use = true;
        }
    }  
}

void physics_sprite::left_collision(sprite* other)
{
    //if we come from the left, stop us going into the object
    //other.y + 1 just add more of a bigger boundary for the player to hit
	if ((sr.x - other->get_sr()->w) > other->get_sr()->x && sr.x < ((other->get_sr()->x + other->get_sr()->w) + PLATFORM_WIDTH_OFFSET) &&
		((sr.y + SPRITE_COL_Y_OFFSET) > (sr.y + 1) &&
			(sr.y - SPRITE_COL_Y_OFFSET) < ((other->get_sr()->y + other->get_sr()->h))))
                sr.x += 1;
}

void physics_sprite::right_collision(sprite* other)
{
    //if we come from the right, stop us going into the object
	if ((sr.x + sr.w) < other->get_sr()->x && (sr.x + sr.w) > (other->get_sr()->x - PLATFORM_WIDTH_OFFSET) &&
		((sr.y + SPRITE_COL_Y_OFFSET) > (other->get_sr()->y) &&
			(sr.y - SPRITE_COL_Y_OFFSET) < ((other->get_sr()->y + other->get_sr()->h))))
                sr.x -= 1;
}

void physics_sprite::down_collision(sprite* other)
{
    if ((sr.y + sr.h) >= other->get_sr()->y && 
        (sr.x + sr.w) > other->get_sr()->x && 
            sr.x < (other->get_sr()->x + other->get_sr()->w)){
                //if (!can_jump)
                  //  sound_jump_land.play();
                sr.y -= 2;
            }
}

void physics_sprite::up_collision(sprite* other)
{
    if (sr.y >= (other->get_sr()->y + other->get_sr()->h) && 
        (sr.x + sr.w) > other->get_sr()->x && 
            sr.x < (other->get_sr()->x + other->get_sr()->w))
                sr.y += 2;
}

physics_sprite::~physics_sprite()
{

}

#endif