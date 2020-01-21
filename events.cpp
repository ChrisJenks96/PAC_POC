#include "events.h"

void event_bkg_id_update(int* bkg_old_id, int* bkg_old_size, int* bkg_old_id_offset,
	int* bkg_id, int* bkg_size, int* bkg_id_offset)
{
	switch (*bkg_id)
	{
		case BKG_0_ID:
		{
			*bkg_old_id = *bkg_id;
			*bkg_old_size = *bkg_size;
			*bkg_old_id_offset = *bkg_id_offset;
			*bkg_id = BKG_1_ID; 
			*bkg_size = BKG_1_SIZE; 
			break;
		}

		case BKG_1_ID:
		{
			*bkg_old_id = *bkg_id;
			*bkg_old_size = *bkg_size;
			*bkg_old_id_offset = *bkg_id_offset;
			if (*bkg_id_offset == 0){
				*bkg_id = BKG_3_ID; 
				*bkg_size = BKG_3_SIZE; 
			}
			else if (*bkg_id_offset == 1){
				*bkg_id = BKG_2_ID; 
				*bkg_size = BKG_2_SIZE; 
			}
			break;
		}

		case BKG_2_ID:
		{
			*bkg_old_id = *bkg_id;
			*bkg_old_size = *bkg_size;
			*bkg_old_id_offset = *bkg_id_offset;
			*bkg_id = BKG_3_ID; 
			*bkg_size = BKG_3_SIZE; 
			break;
		}

		case BKG_3_ID:
		{
			*bkg_old_id = *bkg_id;
			*bkg_old_size = *bkg_size;
			*bkg_old_id_offset = *bkg_id_offset;
			*bkg_id = BKG_4_ID; 
			*bkg_size = BKG_4_SIZE; 
			break;
		}

		case BKG_4_ID:
		{
			*bkg_old_id = *bkg_id;
			*bkg_old_size = *bkg_size;
			*bkg_id = BKG_5_ID; 
			*bkg_size = BKG_5_SIZE; 
			break;
		}

		case BKG_5_ID:
		{
			*bkg_old_id = *bkg_id;
			*bkg_old_size = *bkg_size;
			*bkg_id = BKG_6_ID; 
			*bkg_size = BKG_6_SIZE; 
			break;
		}

		case BKG_6_ID:
		{
			*bkg_old_id = *bkg_id;
			*bkg_old_size = *bkg_size;
			*bkg_id = BKG_7_ID; 
			*bkg_size = BKG_7_SIZE; 
			break;
		}

		case BKG_7_ID:
		{
			*bkg_old_id = *bkg_id;
			*bkg_old_size = *bkg_size;
			*bkg_id = BKG_8_ID; 
			*bkg_size = BKG_8_SIZE; 
			break;
		}

		case BKG_8_ID:
		{
			*bkg_old_id = *bkg_id;
			*bkg_old_size = *bkg_size;
			*bkg_id = BKG_9_ID; 
			*bkg_size = BKG_9_SIZE; 
			break;
		}

		case BKG_9_ID:
		{
			*bkg_old_id = *bkg_id;
			*bkg_old_size = *bkg_size;
			*bkg_id = BKG_10_ID; 
			*bkg_size = BKG_10_SIZE; 
			break;
		}

		case BKG_10_ID:
		{
			/**bkg_old_id = *bkg_id;
			*bkg_old_size = *bkg_size;
			*bkg_id = BKG_6_ID; 
			*bkg_size = BKG_6_SIZE;*/
			break;
		}
	}
}

event_seq* events_pos_parse(char* fn, int scr_w, int scr_h)
{
	FILE* f;
	int curr_event = 0, index = 0, i, j;
	//tmp buffers for data
	char buff[128], empty[128];
	event_seq* e = (event_seq*)malloc(sizeof(event_seq));
	e->num_events = 0;
	f = fopen(fn, "rb");
	if (f == NULL)
		return NULL;
	else
	{
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
					
					//adapt coordinates for different platforms
					//all coordinates are based off the 800 x 600 default
					float new_coord_x = (float)((float)scr_w / 800.0f); 
					float new_coord_y = (float)((float)scr_h / 600.0f); 
					e->es1[index].pos_x *= new_coord_x;
					e->es1[index].pos_y *= new_coord_y;
					e->es1[index].size_x *= new_coord_x;
					e->es1[index].size_y *= new_coord_y;

					e->es1[index].es2 = (event_seq_child2*)malloc(sizeof(event_seq_child2) * e->es1[index].sub_events);
					for (j = 0; j < e->es1[index].sub_events; j++)
					{
						fgets(buff, 128, f);
						sscanf(buff, "%s", &empty[0]);
						if (strcmp(empty, "text") == 0)
						{
							sscanf(buff, "%s %i", &empty[0], &e->es1[index].es2[j].str_num_cols);
							//buff[5] removes text preface and -1 removes string formatting at end
							int str_end_id = (str_find(buff, '>', 64) + 1);
							strncpy(&e->es1[index].es2[j].id_str[0], &buff[7], str_end_id - 7);
						}

						else
						{
							e->es1[index].es2[j].str_num_cols = -1; //not relevant here
							sscanf(buff, "%s %s", &empty[0], &e->es1[index].es2[j].id_str[0]);
						}

						e->es1[index].es2[j].done = false;
						//assign proper id to event
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