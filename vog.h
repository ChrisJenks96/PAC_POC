#ifndef VOG_FORMAT_H
#define VOG_FORMAT_H

#ifdef _WIN32
	#include <SDL.h>
#elif _PSP
	#include <SDL/SDL.h>
#endif

#include "util.h"
#include "sound.h"

#define XMD_H
extern "C" {
	#include <jpeglib.h>
	#include <setjmp.h>
}

#define VOG_RGB 3

extern unsigned char* vog_data;
extern SDL_Surface* vog_video_surface;
extern SDL_Rect vog_dest;
extern FILE* f_vog;
extern int vog_num_frames;
extern int vog_width;
extern int vog_height;
extern int f_vog_curr_frame;
extern int f_vog_block_size;
extern bool f_vog_done;
extern bool vog_play;

extern struct jpeg_decompress_struct cinfo;	
extern struct my_error_mgr jerr;
extern JSAMPARRAY buffer;		/* Output row buffer */
extern int row_stride;		/* physical row width in output buffer */

int vog_setup(const char* fn, const char* s_fn, int scr_w, int scr_h);
int vog_get_frame_data();
void vog_update(SDL_Surface* scr);

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

GLOBAL(int) read_JPEG_file ();
METHODDEF(void) my_error_exit (j_common_ptr cinfo);

#endif