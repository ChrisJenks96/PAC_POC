#define LOADBMP_IMPLEMENTATION
#include "load_bmp.h"
#include <stdio.h>
#include <string.h>
#include "dirent.h"

#include <png.h>

#define XMD_H
extern "C" {
	#include <jpeglib.h>
	#include <setjmp.h>
}

JSAMPLE* image_buffer;	/* Points to large array of R,G,B-order data */
int image_height;	/* Number of rows in image */
int image_width;		/* Number of columns in image */

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}


GLOBAL(int)
read_JPEG_file (char * filename)
{
  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;
  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct my_error_mgr jerr;
  /* More stuff */
  FILE * infile;		/* source file */
  JSAMPARRAY buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */

  /* In this example we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */

  image_width = cinfo.output_width;
  image_height = cinfo.output_height;
  image_buffer = (JSAMPLE*)malloc(sizeof(JSAMPLE) * (3 * image_width * image_height));

  int i, src, dest = 0;
  while (cinfo.output_scanline < cinfo.output_height) {
	  src = 0;
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
    //put_scanline_someplace(buffer[0], row_stride);
	for(i=0;i<cinfo.output_width;i++)
    {
		image_buffer[dest*3+2]=buffer[0][src*3+0];
		image_buffer[dest*3+1]=buffer[0][src*3+1];
		image_buffer[dest*3+0]=buffer[0][src*3+2];
		src++;
		dest++;
	}
  }

  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */
  fclose(infile);

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */
  return 1;
}

int main(int argc, char** argv)
{
	//drag the folder into the executable
	//argv[1] folder
	//argv[2] num of bitmaps
	//argv[3] width
	//argv[4] height
	//argv[5] music name

	if (argc > 5)
	{
		DIR *dir;
		struct dirent *ent;
		long int nbytes;
		unsigned char* image_buffer;
		unsigned char* b;
		FILE* f;
		char fn_buff[32];
		int count = 0;
		int f_width, f_height, f_max_count;
		sscanf(argv[2], "%i", &f_max_count);
		sscanf(argv[3], "%i", &f_width);
		sscanf(argv[4], "%i", &f_height);

		FILE* f2 = fopen(argv[5], "rb");
		if (f2)
		{
			fseek(f2, 0, SEEK_END);
			nbytes = ftell(f2);
			fseek(f2, 0, SEEK_SET);
			b = (unsigned char*)malloc(nbytes);
			fread(&b[0], nbytes, 1, f2);
			fclose(f2);
		}

		if ((dir = opendir (argv[1])) != NULL) 
		{
			f = fopen("DUMP.vog", "wb");
			fwrite(&f_max_count, 4, 1, f);
			fwrite(&f_width, 4, 1, f);
			fwrite(&f_height, 4, 1, f);
			//write the music to the file w/ size
			fwrite(&nbytes, 4, 1, f);
			fwrite(&b[0], nbytes, 1, f);
			free(b);

			printf("beginning conversion....\n");
			/* print all the files and directories within directory */
			while ((ent = readdir (dir)) != NULL)
			{
				if (ent->d_namlen > 2)
				{
					strcpy(&fn_buff[0], argv[1]);
					strcpy(&fn_buff[strlen(fn_buff)], ent->d_name);
					printf("%s\n", fn_buff);
					f2 = fopen(fn_buff, "rb");
					if (f2)
					{
						fseek(f2, 0, SEEK_END);
						nbytes = ftell(f2);
						fseek(f2, 0, SEEK_SET);
						b = (unsigned char*)malloc(nbytes);
						printf("%i\n", nbytes);
						fread(&b[0], nbytes, 1, f2);
						//write size of png then write jpg
						fwrite(&nbytes, 4, 1, f);
						fwrite(&b[0], nbytes, 1, f);
						free(b);
					}

					else
						printf("ERROR: %s\n", fn_buff);
					count++;

					fclose(f2);
					if (count >= f_max_count)
						break;
				}
			}

			closedir (dir);
			fclose(f);
			printf("finished conversion....\n");
		} 
	
		else 
		  return -1;
	}

	else
	{
		printf("error: [folder name] [num bitmaps] [width] [height] [ogg sound]\n");
		return -1;
	}

	return 0;
}