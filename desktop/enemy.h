#ifndef ENEMY_H
#define ENEMY_H

#include "core.h"
#include "physics_sprite.h"

class enemy : public sprite
{
	public:
		void update(core* mc, physics_sprite* player);
		void reset_pos();
		void world_bound_check(short scr_w, short scr_h);
	private:
		short old_dir_x[2];
		short old_dir_y[2];
		short dir_x;
		short dir_y;
		unsigned char cycle;
};

#endif