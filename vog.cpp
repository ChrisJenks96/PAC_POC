#include "vog.h"

char* vog_data;
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
bool vog_blank_flag = false;

struct jpeg_decompress_struct cinfo;	
struct my_error_mgr jerr;
JSAMPARRAY buffer;		/* Output row buffer */
int row_stride;		/* physical row width in output buffer */

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
	vog_data = (char*)malloc(f_vog_block_size);
	#ifdef _PSP
		int load, start, status;
		load = load_module("flash0:/kd/avcodec.prx", 0, NULL);
		start = sceKernelStartModule(load, 0, NULL, &status, NULL);
		sceJpegInitMJpeg();
		sceJpegCreateMJpeg(vog_width, vog_height);
	#elif _WIN32
		/* Now we can initialize the JPEG decompression object. */
		jpeg_create_decompress(&cinfo);
		/* JSAMPLEs per row in output buffer */
		row_stride = vog_width * VOG_RGB;
		/* Make a one-row-high sample array that will go away when done with image */
		buffer = (*cinfo.mem->alloc_sarray)
			((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	#endif

	vog_video_surface = NULL;

	//default shite... change later
	vog_dest.h = (Uint16)vog_height;
	vog_dest.w = (Uint16)vog_width;
	vog_dest.x = (scr_w - vog_width) / 2;
	vog_dest.y = (scr_h - vog_height) / 2;

	//create shell surface once, only update data
	vog_video_surface = SDL_CreateRGBSurfaceFrom(NULL, 
		vog_width, vog_height, VOG_BPP, VOG_RGB * vog_width, 
			0x000000ff, 0x0000ff00, 0x00ff0000, 0);

	vog_play = true;
	return 0;
}

int vog_get_frame_data()
{
	if (f_vog_curr_frame < vog_num_frames){
		read_JPEG_file();
		f_vog_curr_frame++;
	}

	else
	{
		#ifdef _PSP
			sceJpegDeleteMJpeg();
			sceJpegFinishMJpeg();
		#endif
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
		SDL_Delay(33);
	#endif
	if (vog_get_frame_data() != -1)
	{
		vog_video_surface->pixels = vog_data;
		//SDL_BlitSurface(vog_video_surface, NULL, scr, &vog_dest);
		_SDL_UpperBlit(vog_video_surface, NULL, scr, &vog_dest);
	}
}

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

#ifdef _PSP
	SceUID load_module(const char *path, int flags, int type)
	{
		SceKernelLMOption option;
		SceUID mpid;

		/* If the type is 0, then load the module in the kernel partition, otherwise load it
		   in the user partition. */
		if (type == 0) {
			mpid = 1;
		} else {
			mpid = 2;
		}

		memset(&option, 0, sizeof(option));
		option.size = sizeof(option);
		option.mpidtext = mpid;
		option.mpiddata = mpid;
		option.position = 0;
		option.access = 1;

		return sceKernelLoadModule(path, flags, &option);
	}

	void jpg2buf()
	{
		int next_seek, old_seek;
		old_seek = ftell(f_vog);
		fread(&next_seek, 4, 1, f_vog);
		char* databuf = (char*)malloc(next_seek);
		fread(&databuf[0], next_seek, 1, f_vog);
		//sceJpegInitMJpeg();
		//sceJpegCreateMJpeg(vog_width, vog_height);
		int res = sceJpegDecodeMJpeg((u8*)databuf, next_seek, vog_data, 0);
		free(databuf);
		//move onto the next JPEG
		fseek(f_vog, (old_seek + next_seek) + 4, SEEK_SET);
	}
#endif

GLOBAL(int) read_JPEG_file()
{
	#ifdef _PSP
		jpg2buf();
		return 1;
	#elif _WIN32
		int next_seek, old_seek;
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;
		/* Establish the setjmp return context for my_error_exit to use. */
		if (setjmp(jerr.setjmp_buffer)) {
			/* If we get here, the JPEG code has signaled an error.
				* We need to clean up the JPEG object, close the input file, and return.
				*/
			jpeg_destroy_decompress(&cinfo);
			return 0;
		}

		/* Now we can initialize the JPEG decompression object. */
		jpeg_create_decompress(&cinfo);

		old_seek = ftell(f_vog);
		fread(&next_seek, 4, 1, f_vog);
		jpeg_stdio_src(&cinfo, f_vog);
		jpeg_read_header(&cinfo, FALSE);
		jpeg_start_decompress(&cinfo);

		int i, src = 0, dest = 0;
		while (cinfo.output_scanline < cinfo.output_height) 
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);
			memcpy(&vog_data[dest * row_stride], &buffer[0][src * row_stride], row_stride);
			dest++;
		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		//move onto the next JPEG
		fseek(f_vog, (old_seek + next_seek) + 4, SEEK_SET);
		return 1;
	#endif
}
