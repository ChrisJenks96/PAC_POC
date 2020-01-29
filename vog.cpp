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

int x, y;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep * row_pointers;
unsigned int row_bytes;

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
	vog_data = (unsigned char*)malloc(f_vog_block_size);
	vog_video_surface = NULL;

	//default shite... change later
	vog_dest.h = (Uint16)vog_height;
	vog_dest.w = (Uint16)vog_width;
	vog_dest.x = (scr_w - vog_width) / 2;
	vog_dest.y = (scr_h - vog_height) / 2;

	//create shell surface once, only update data
	vog_video_surface = SDL_CreateRGBSurfaceFrom(NULL, 
		vog_width, vog_height, 24, VOG_RGB * vog_width, 
			0x000000ff, 0x0000ff00, 0x00ff0000, 0);

	 row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * vog_height);
    for (y=0; y<vog_height; y++)
		row_pointers[y] = (png_byte*)malloc(vog_width * VOG_RGB);

	vog_play = true;
	return 0;
}

int vog_get_frame_data()
{
	if (f_vog_curr_frame < vog_num_frames)
	{
		png_read_end(png_ptr, info_ptr);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		read_png_file();

		// note that png is ordered top to
		// bottom, but OpenGL expect it bottom to top
		// so the order or swapped
		for (int i = 0; i < vog_height; i++)
			memcpy(vog_data+(row_bytes * (vog_height-1-i)), row_pointers[i], row_bytes);
		
		f_vog_curr_frame++;
	}

	else
	{
		for (y=0; y<vog_height; y++)
			free(row_pointers[y]);
		free(row_pointers);
		png_read_end(png_ptr, info_ptr);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
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
	if (!sound_isplaying()){
		bool tmp_bool = true;
		sound_play(&music, &tmp_bool, false); 
	}

	#ifdef _WIN32
		SDL_Delay(10);
	#endif
	if (vog_get_frame_data() != -1)
	{
		vog_video_surface->pixels = vog_data;
		SDL_BlitSurface(vog_video_surface, NULL, scr, &vog_dest);
		SDL_Flip(scr);
	}
}

void read_png_file()
{
    char header[8];    // 8 is the maximum size that can be checked

    fread(header, 1, 8, f_vog);
  
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, f_vog);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    png_read_update_info(png_ptr, info_ptr);

	png_read_image(png_ptr, row_pointers);
	row_bytes = png_get_rowbytes(png_ptr, info_ptr);
}
