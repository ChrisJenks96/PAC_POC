#ifndef EVENTS_H
#define EVENTS_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define SOUND_ID 0
#define VIDEO_ID 1
#define TEXT_ID 2
#define NEXT_SCENE_ID 3
#define START_ANIM_ID 4
#define END_ANIM_ID 5

#pragma pack(push, 1)
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
#pragma pack(pop, 1)

event_seq* events_pos_parse(char* fn, int scr_w, int scr_h);
bool events_pos_update(event_seq_child* e, int mx, int my);
void events_pos_destroy(event_seq* e);

//game code
//start zone looking at house, truck
#define BKG_0_ID 0
#define BKG_0_SIZE 1

//looking further in
#define BKG_1_ID (BKG_0_ID + BKG_0_SIZE)
#define BKG_1_SIZE 2

//truck cockpit and fuel
#define BKG_2_ID (BKG_1_ID + BKG_1_SIZE)
#define BKG_2_SIZE 2

//the house
#define BKG_3_ID (BKG_2_ID + BKG_2_SIZE)
#define BKG_3_SIZE 2

#endif