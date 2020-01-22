#ifndef EVENTS_H
#define EVENTS_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "util.h"

#define SOUND_ID 0
#define VIDEO_ID 1
#define TEXT_ID 2
#define NEXT_SCENE_ID 3
#define START_ANIM_ID 4
#define END_ANIM_ID 5

typedef struct event_seq_child2
{
	//have we done the event yet?
	bool done;
	int id;
	int str_num_cols;
	char id_str[64];
} event_seq_child2;

typedef struct event_seq_child
{
	int pos_x;
	int pos_y;
	int size_x;
	int size_y;
	int sub_events;
	event_seq_child2* es2;
} event_seq_child;

typedef struct event_seq
{
	int num_events;
	event_seq_child* es1;
} event_seq;

void event_bkg_id_update(int* bkg_old_id, int* bkg_old_size, int* bkg_old_id_offset,
	int* bkg_id, int* bkg_size, int* bkg_id_offset);
event_seq* events_pos_parse(char* fn, int scr_w, int scr_h, 
	int tex_w, int tex_h, int offset_x, int offset_y);
bool events_pos_update(event_seq_child* e, int mx, int my);
void events_pos_destroy(event_seq* e);

//game code

//main_camera_1
//start zone looking at house, truck
#define BKG_0_ID 0
#define BKG_0_SIZE 1

//main_camera_1_1
//looking further in
#define BKG_1_ID (BKG_0_ID + BKG_0_SIZE)
#define BKG_1_SIZE 2

//main_camera_1_2
//truck cockpit and fuel
#define BKG_2_ID (BKG_1_ID + BKG_1_SIZE)
#define BKG_2_SIZE 2

//main_camera_1_3
//the house
#define BKG_3_ID (BKG_2_ID + BKG_2_SIZE)
#define BKG_3_SIZE 2

//main_camera_2
//entrance to the big cave through the house
#define BKG_4_ID (BKG_3_ID + BKG_3_SIZE)
#define BKG_4_SIZE 1

//main_camera_2_1
//moving in on the portable lift
#define BKG_5_ID (BKG_4_ID + BKG_4_SIZE)
#define BKG_5_SIZE 1

//main_camera_2_2
//moving in on the portable lift (again)
#define BKG_6_ID (BKG_5_ID + BKG_5_SIZE)
#define BKG_6_SIZE 1

//main_camera_2_3
//emergency buttons
#define BKG_7_ID (BKG_6_ID + BKG_6_SIZE)
#define BKG_7_SIZE 1

//main_camera_2_4
//step onto the portable lift
#define BKG_8_ID (BKG_7_ID + BKG_7_SIZE)
#define BKG_8_SIZE 1

//main_camera_2_5
//got to the top of the cave on the lift
#define BKG_9_ID (BKG_8_ID + BKG_8_SIZE)
#define BKG_9_SIZE 1

//main_camera_2_6
//got to the top of the cave on the lift
#define BKG_10_ID (BKG_9_ID + BKG_9_SIZE)
#define BKG_10_SIZE 1

#endif