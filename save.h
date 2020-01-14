#ifndef SAVE_H
#define SAVE_H

#include <stdio.h>
#include <string.h>
#include <malloc.h>

//the overall scene we are parsing to get the event data
extern int gs_scene_id;
//the bkg id from the current scene event
extern int gs_bkg_id;
extern int gs_bkg_size;
//for chosing the right different event with the same bkg
extern int gs_bkg_id_offset;
//save load the event file name
extern char scn[32];

char* scene_list();
bool game_load();
bool game_save();

#endif