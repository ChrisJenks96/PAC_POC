#ifndef LEVEL_H
#define LEVEL_H

//number of tiles in the scene
const static unsigned char lvl_0_tile_size = 7;
//the tiles xy position
const static unsigned short lvl_0_tile_xy[] = 
{
    64, 110,
    96, 125,
    128, 130,
    165, 140,
    192, 135,
    236, 140,
    280, 155
};

unsigned int lvl_0_tile_inc_count = 0;

typedef struct lvl_next
{
	const char bits[8]; //the 7 bits for the platform movement
	bool up; //1 for up, 0 for down
} lvl_next;

const static lvl_next lvl_0_tile_inc[] = 
{
    //only 7 platforms, hence the pattern
	"0000101", 1,
    "0001001", 0,
    "0110000", 1
};

#endif