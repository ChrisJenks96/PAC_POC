#include "anim.h"

void anim_parse_pos(char* fn, anim_pos* v, int winx, int winy)
{
	FILE* f;
	int c = 0;
	int32_t x, y;
	char v1[5];
	char buffer[64];

	#ifdef _PSP
		//define aspect ratio between default scr_width/height and targetted platform scr_width/height
		float x_correction, y_correction;
	#endif

	f = fopen(fn, "rb");
	while(fgets(buffer, 64, f)) 
	{
		if (c == 0)
		{
			strncpy(v1, &buffer[1], 5);
			sscanf(v1, "%i", &x);
			strncpy(v1, &buffer[8], 5);
			sscanf(v1, "%i", &y);
		}

		else
		{
			strncpy(v1, &buffer[1], 5);
			sscanf(v1, "%i", &v[c-1].xy.x);
			strncpy(v1, &buffer[8], 5);
			sscanf(v1, "%i", &v[c-1].xy.y);
			//unity inversed viewport window (opengl/dx)
			v[c-1].xy.y = winy - v[c-1].xy.y;
			/*#ifdef _PSP
				//get aspect ratio for default windows version
				x_correction = (float)v[c-1].xy.x / 800.0f;
				y_correction = (float)v[c-1].xy.y / 600.0f;
				//offset by psp width/height
				v[c-1].xy.x = (int)((float)winx * x_correction);
				v[c-1].xy.y = (int)((float)winy * y_correction);;
			#endif*/
			v[c-1].size.x = x;
			v[c-1].size.y = y;
		}

		c++;
	}

	fclose(f);
}