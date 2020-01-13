#ifndef ANIM_H
#define ANIM_H

#include <stdio.h>
#include "util.h"

typedef struct anim_pos
{
	vec2_int32 xy;
	vec2_int32 size;
} anim_pos;

void anim_parse_pos(char* fn, anim_pos* v, int winx, int winy);

#endif
