#ifndef ENEMY_H
#define ENEMY_H

#include "core.h"
#include "physics_sprite.h"

class enemy : public sprite
{
	public:
		void update(core* mc, sprite* player);
		void reset_pos();
		void world_bound_check(int scr_w, int scr_h);
	private:
		short old_dir_x[2];
		short old_dir_y[2];
		short dir_x;
		short dir_y;
		unsigned char cycle;
};

#endif