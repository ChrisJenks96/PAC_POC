#ifndef PHYSICS_SPRITE
#define PHYSICS_SPRITE

#include "sprite.h"

#define GRAVITY_VALUE 1
#define GRAVITY_WATER_VALUE 1
#define SPRITE_COL_Y_OFFSET 8 //main_sprite.width >> 2; //div by 2
#define PLATFORM_WIDTH_OFFSET 2 //extra space on the x for the platform for the player to fall off (game feature)
#define MAX_JUMP_HEIGHT 20

class physics_sprite : public sprite
{
	public:
		physics_sprite()
		{
			wb_check = false; in_water = false; gravity_use = true; can_jump = false; is_jump = false;
			old_jump_y = 0; old_y = 0; speedx = 0; speedy = 0;
		}

		bool collision_update(physics_sprite* other); 
		void world_bound_check(short scr_w, short scr_h);
		void left_collision(sprite* other);
		void right_collision(sprite* other);
		void down_collision(sprite* other);
		void up_collision(sprite* other);
		void in_water_collision_update(short y);
		void out_of_world_update(short scr_h, const unsigned short* lvl_data);
		void zero();
		void jump_update(bool jump_key_press);
		void update(short scr_w, short scr_h);
		~physics_sprite();
		short speedx, speedy;
	private:
		bool is_jump;
		bool can_jump;
		short old_jump_y;
		short old_y;
		bool gravity_use;
		bool in_water;
		bool wb_check;
};

#endif