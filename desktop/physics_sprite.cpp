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
	if (wb_check_flag)
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

void physics_sprite::update(short scr_w, short scr_h)
{
	zero();

	if (gravity_use_flag)
	{
		if (!in_water_flag)
			speedy = GRAVITY_VALUE;
		else
			speedy = GRAVITY_WATER_VALUE;
	}

	sr.x += speedx;
	sr.y += speedy;

	world_bound_check(scr_w, scr_h);
}

void physics_sprite::left_collision(sprite* other)
{
    //if we come from the left, stop us going into the object
    //other.y + 1 just add more of a bigger boundary for the player to hit
	if ((sr.x - other->get_sr()->w) > other->get_sr()->x && sr.x < ((other->get_sr()->x + other->get_sr()->w) + PLATFORM_WIDTH_OFFSET) &&
		((sr.y + SPRITE_COL_Y_OFFSET) > (sr.y + 1) &&
			(sr.y - SPRITE_COL_Y_OFFSET) < ((sr.y + other->get_sr()->h))))
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