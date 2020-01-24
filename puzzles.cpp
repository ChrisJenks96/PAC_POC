#include "puzzles.h"

int puzzle_id = -1;

puzzle_seq* puzzle_event_parse(char* fn, int scr_w, int scr_h, int tex_w, int tex_h, int offset_x, int offset_y)
{
	FILE* f;
	int curr_event = 0, sub_index = 0, i, j;
	//tmp buffers for data
	char buff[128], empty[128];
	puzzle_seq* p = (puzzle_seq*)malloc(sizeof(puzzle_seq));
	p->num_events = 0;
	f = fopen(fn, "rb");
	if (f == NULL)
		return NULL;
	else
	{
		fgets(buff, 128, f);
		sscanf(buff, "%i", &curr_event);
		p->es1 = (puzzle_seq_child*)malloc(sizeof(puzzle_seq_child) * curr_event);
		p->es1_name = (char**)malloc(sizeof(char**) * curr_event);
		while (!feof(f))
		{
			fgets(buff, 128, f);
			sscanf(buff, "%s %i", &empty[0], &curr_event);
			if ((strcmp(buff, "\r\n") != 0))
			{
				p->num_events += curr_event;
				for (i = 0; i < curr_event; i++)
				{
					p->es1[i].done = false;
					//set to NULL for checks later
					p->es1[i].ns = NULL;

					int len = _strlen(empty);
					p->es1_name[i] = (char*)malloc(sizeof(char) * len);
					strcpy(p->es1_name[i], &empty[0]);

					fgets(buff, 128, f);
					sscanf(buff, "%s", &empty[0]);
					if (strcmp(empty, "inv") == 0)
					{
						//inventory task
						sscanf(buff, "%s %s %i %i %i %i %i", &empty[0], 
							&p->es1[i].inv_id[0], &p->es1[i].pos_x, 
							&p->es1[i].pos_y, &p->es1[i].size_x, 
								&p->es1[i].size_y, &p->es1[i].sub_events);

						//adapt coordinates for different platforms
						//all coordinates are based off the 800 x 600 default
						float new_coord_x = (float)((float)tex_w / 800.0f); 
						float new_coord_y = (float)((float)tex_h / 600.0f); 
						p->es1[i].pos_x *= new_coord_x;
						p->es1[i].pos_x += offset_x;
						p->es1[i].pos_y *= new_coord_y;
						p->es1[i].pos_y += offset_y;
						p->es1[i].size_x *= new_coord_x;
						p->es1[i].size_y *= new_coord_y;

						//revert back to the current file seek after checking the next first key
						int last_seek = ftell(f);
						fgets(buff, 128, f);
						sscanf(buff, "%s", &empty[0]);
						//fseek(f, last_seek, SEEK_SET);
						//move the seek pointer back to before the next line
						if (strcmp(empty, "next_bkg") == 0)
							p->es1[i].ns = (puzzle_seq_next_scene*)malloc(sizeof(puzzle_seq_next_scene) * p->es1[i].sub_events);

						for (j = 0; j < p->es1[i].sub_events; j++)
						{
							if (strcmp(empty, "next_bkg") == 0)
							{
								sscanf(buff, "%s %s %s", &empty[0], &p->es1[i].ns[sub_index].before_bkg[0],
								&p->es1[i].ns[sub_index].after_bkg[0]);
								fgets(buff, 128, f);
								sub_index++;
							}
						}
					}
				}
			}
		}

		fclose(f);
		return p;
	}
}

void puzzle_event_destroy(puzzle_seq* p)
{
	if (p != NULL)
	{
		int i;
		for (i = 0; i < p->num_events; i++){
			if (p->es1[i].ns != NULL){
				free(p->es1[i].ns);
				p->es1[i].ns = NULL;
			}
		}

		free(p->es1_name);
		p->es1_name = NULL;
		free(p->es1);
		p->es1 = NULL;
	}
}

int puzzle_event_find(puzzle_seq* p, char* s)
{
	int i;
	for (i = 0; i < p->num_events; i++){
		if (strcmp(p->es1_name[i], s) == 0){
			return i;
		}
	}

	return -1;
}

char* puzzle_event_update_bkg(puzzle_seq* p, char* s)
{
	int i, j;
	for (i = 0; i < p->num_events; i++){
		for (j = 0; j < p->es1[i].sub_events; j++){
			if (strcmp(p->es1[i].ns[j].before_bkg, s) == 0){
				return p->es1[i].ns[j].after_bkg;
			}
		}
	}

	return NULL;
}

bool puzzle_event_update(puzzle_seq* p, int id, int mx, int my)
{
	if (mx >= (p->es1[id].pos_x) && mx <= (p->es1[id].pos_x + p->es1[id].size_x)){
		if (my >= (p->es1[id].pos_y) && my <= (p->es1[id].pos_y + p->es1[id].size_y)){
			return true;
		}
	}
	return false;
}