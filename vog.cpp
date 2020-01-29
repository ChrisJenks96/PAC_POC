#include "vog.h"

unsigned char* vog_data;
SDL_Surface* vog_video_surface;
SDL_Rect vog_dest;
FILE* f_vog;
int vog_num_frames = 0;
int vog_width = 0;
int vog_height = 0;
//tracking the framesi n the vid
int f_vog_curr_frame = 0;
//used to iterate through the blocks of data
int f_vog_block_size = 0;
//when we've got to the end of the file, end this..
bool f_vog_done = false;
bool vog_play = false;

int vog_setup(const char* fn, const char* s_fn, int scr_w, int scr_h)
{
	f_vog = fopen(fn, "rb");
	if (!f_vog)
		return -1;

	if (!sound_load(&music, s_fn))
		return -1;

	fread(&vog_num_frames, 4, 1, f_vog);
	fread(&vog_width, 4, 1, f_vog);
	fread(&vog_height, 4, 1, f_vog);

	f_vog_done = false;
	f_vog_block_size = VOG_RGB * vog_width * vog_height;
	vog_data = NULL;
	vog_video_surface = NULL;

	//default shite... change later
	vog_dest.h = (Uint16)vog_height;
	vog_dest.w = (Uint16)vog_width;
	vog_dest.x = (scr_w - vog_width) / 2;
	vog_dest.y = (scr_h - vog_height) / 2;
	vog_play = true;
	return 0;
}

int vog_get_frame_data()
{
	if (f_vog_curr_frame < vog_num_frames)
	{
		if (vog_data != NULL)
			free(vog_data);
		vog_data = (unsigned char*)malloc(f_vog_block_size);
		fread(&vog_data[0], f_vog_block_size, 1, f_vog);
		f_vog_curr_frame++;
	}

	else
	{
		SDL_FreeSurface(vog_video_surface);
		free(vog_data);
		fclose(f_vog);
		f_vog_done = true;
		vog_play = false;
		return -1;
	}

	return 0;
}

void vog_update(SDL_Surface* scr)
{
	bool tmp_bool = true;
	
	sound_play(&music, &tmp_bool, false); 
	while (!f_vog_done)
	{
		#ifdef _WIN32
			SDL_Delay(31);
		#endif
		if (vog_get_frame_data() != -1)
		{
			if (vog_video_surface != NULL)
				SDL_FreeSurface(vog_video_surface);
			vog_video_surface = SDL_CreateRGBSurfaceFrom(vog_data, 
				vog_width, vog_height, 24, VOG_RGB * vog_width, 
					0x000000ff, 0x0000ff00, 0x00ff0000, 0);
			SDL_BlitSurface(vog_video_surface, NULL, scr, &vog_dest);
			SDL_Flip(scr);
		}
	}
}
