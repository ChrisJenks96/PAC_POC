#ifndef PUZZLES_H
#define PUZZLES_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "experimental.h"

extern int puzzle_id;

typedef struct puzzle_seq_next_scene
{
	char before_bkg[32];
	char after_bkg[32];
} puzzle_seq_next_scene;

typedef struct puzzle_seq_child
{
	bool done;
	char inv_id[32];
	int pos_x;
	int pos_y;
	int size_x;
	int size_y;
	int sub_events;
	puzzle_seq_next_scene* ns;
} puzzle_seq_child;

typedef struct puzzle_seq
{
	int num_events;
	char** es1_name;
	puzzle_seq_child* es1;
} puzzle_seq;

puzzle_seq* puzzle_event_parse(char* fn, int scr_w, int scr_h, 
	int tex_w, int tex_h, int offset_x, int offset_y);
void puzzle_event_destroy(puzzle_seq* p);
int puzzle_event_find(puzzle_seq* p, char* s);
char* puzzle_event_find_bkg(puzzle_seq* p, char* s);
bool puzzle_event_update(puzzle_seq* p, int id, int mx, int my);

#define NO_LIFE_POWER_PUZZLE 0

#endif