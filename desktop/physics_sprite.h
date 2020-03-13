#ifndef PHYSICS_SPRITE
#define PHYSICS_SPRITE

#include "sprite.h"

#define GRAVITY_VALUE 1
#define GRAVITY_WATER_VALUE 0
#define SPRITE_COL_Y_OFFSET 8 //main_sprite.width >> 2; //div by 2
#define PLATFORM_WIDTH_OFFSET 2 //extra space on the x for the platform for the player to fall off (game feature)

class physics_sprite : public sprite
{
	public:
		physics_sprite(){wb_check_flag = false; in_water_flag = false; gravity_use_flag = true;}
		bool collision_update(physics_sprite* other); 
		void world_bound_check(short scr_w, short scr_h);
		void left_collision(sprite* other);
		void right_collision(sprite* other);
		void down_collision(sprite* other);
		void up_collision(sprite* other);
		void zero();
		void update(short scr_w, short scr_h);
		~physics_sprite();
	private:
		bool gravity_use_flag;
		bool in_water_flag;
		bool wb_check_flag;
		short speedx, speedy;
};

#endif