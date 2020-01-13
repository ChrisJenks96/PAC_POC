#include "events.h"

event_seq* events_pos_parse(char* fn, int scr_w, int scr_h)
{
	FILE* f;
	int curr_event = 0, index = 0, i, j;
	char buff[128], empty[128];
	event_seq* e = (event_seq*)malloc(sizeof(event_seq));
	e->num_events = 0;
	f = fopen(fn, "rb");
	fgets(buff, 128, f);
	sscanf(buff, "%i", &curr_event);
	e->es1 = (event_seq_child*)malloc(sizeof(event_seq_child) * curr_event);
	while (!feof(f))
	{
		fgets(buff, 128, f);
		sscanf(buff, "%s %i", &empty[0], &curr_event);
		if (strcmp(buff, "\r\n") != 0)
		{
			e->num_events += curr_event;
			for (i = 0; i < curr_event; i++)
			{
				fgets(buff, 128, f);
				sscanf(buff, "%s %i %i %i %i %i", &empty[0], &e->es1[index].pos_x, &e->es1[index].pos_y, 
					&e->es1[index].size_x, &e->es1[index].size_y, &e->es1[index].sub_events);
				e->es1[index].es2 = (event_seq_child2*)malloc(sizeof(event_seq_child2) * e->es1[index].sub_events);
				for (j = 0; j < e->es1[index].sub_events; j++)
				{
					fgets(buff, 128, f);
					sscanf(buff, "%s", &empty[0]);
					if (strcmp(empty, "text") == 0){
						sscanf(buff, "%s %i", &empty[0], &e->es1[index].es2[j].str_num_cols);
						//buff[5] removes text preface and -1 removes string formatting at end
						strncpy(&e->es1[index].es2[j].id_str[0], &buff[7], (strlen(buff) - 7) - 2);
						e->es1[index].es2[j].id_str[strlen(e->es1[index].es2[j].id_str)] = 0;
					}

					else
					{
						e->es1[index].es2[j].str_num_cols = -1; //not relevant here
						sscanf(buff, "%s %s", &empty[0], &e->es1[index].es2[j].id_str[0]);
					}

					e->es1[index].es2[j].done = false;

					if (strcmp(empty, "next_scene") == 0)
						e->es1[index].es2[j].id = NEXT_SCENE_ID;
					else if (strcmp(empty, "text") == 0)
						e->es1[index].es2[j].id = TEXT_ID;
					else if (strcmp(empty, "sound") == 0)
						e->es1[index].es2[j].id = SOUND_ID;
					else if (strcmp(empty, "video") == 0)
						e->es1[index].es2[j].id = VIDEO_ID;
					else if (strcmp(empty, "start_anim") == 0)
						e->es1[index].es2[j].id = START_ANIM_ID;
					else if (strcmp(empty, "end_anim") == 0)
						e->es1[index].es2[j].id = END_ANIM_ID;
				}

				index++;
			}
		}
	}

	fclose(f);
	return e;
}

void events_pos_destroy(event_seq* e)
{
	if (e != NULL)
	{
		int i, j;
		for (i = 0; i < e->num_events; i++){
			for (j = 0; j < e->es1[i].sub_events; j++){
				free(e->es1[i].es2);
				e->es1[i].es2 = NULL;
			}
		}

		free(e->es1);
		e->es1 = NULL;
	}
}

bool events_pos_update(event_seq_child* e, int mx, int my)
{
	if (mx >= (e->pos_x) && mx <= (e->pos_x + e->size_x)){
		if (my >= (e->pos_y) && my <= (e->pos_y + e->size_y)){
			return true;
		}
	}
	return false;
}