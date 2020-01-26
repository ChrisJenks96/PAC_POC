#include "util.h"

char str[256];
SDL_Rect debug_r;
uint8_t mouse_down_count = 0;
int mx, my; //mouse coords
SDL_Rect cursor_dest;
float timed_bool_toggle_counter; //0 -> toggle_max
int timed_bool_toggle_max = 30; //20
int game_start_state = 0;

#ifdef _PSP
	SceCtrlData pad;
	SceCtrlLatch latch;

	int isKeyDown(int key) {
		return (latch.uiMake & key);
	}

	int isKeyUp(int key) {
		return (latch.uiBreak & key);
	}
#elif _WIN32
	double PCFreq = 0.0;
	__int64 CounterStart = 0;
	void StartCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);

		PCFreq = double(li.QuadPart)/1000.0;

		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;
	}

	double GetCounter()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart-CounterStart)/PCFreq;
	}
#endif

void pc_mouse_debug_coord(float dt)
{
	sprintf(str, "%d | %d | DeltaTime:%f", mx, my, dt);
	//mx,my already updated per frame
	SDL_WM_SetCaption(str, NULL);
}

void mouse_update(float dt, int scr_w, int scr_h)
{
	#ifdef _WIN32
		SDL_GetMouseState(&mx, &my);
		//dont go outside boundaries
		if (mx > scr_w - 16)
			mx = scr_w - 16;
		if (my > scr_h - 16)
			my = scr_h - 16;
		cursor_dest.x = mx - (cursor_dest.w / 2);
		cursor_dest.y = my - (cursor_dest.h / 2);
	#else
		//deadzone on psp... too freaky w/o
		int x = pad.Lx - 128;
		if (x > -30 && x < 30)
			x = 0;
		int y = pad.Ly - 128;
		if (y > -30 && y < 30)
			y = 0;
		
		cursor_dest.x += x * dt;
		cursor_dest.y += y * dt;
		//dont go outside boundaries
		//psp divide by 2 (32/2=16) doesnt work (Sint /2) (no point casting float and back if we know its size)
		if (cursor_dest.x > scr_w - 16)
			cursor_dest.x = scr_w - 16;
		if (cursor_dest.y > scr_h - 16)
			cursor_dest.y = scr_h - 16;
		//sync var for main menu highlight
		mx = cursor_dest.x;
		my = cursor_dest.y;
	#endif
}

bool timed_bool(bool* k, bool* k_press, float dt, float s)
{
	timed_bool_toggle_counter += dt;
	if (timed_bool_toggle_counter >= s){
		timed_bool_toggle_counter = 0;
		*k_press = false;
		return !*k;
	}

	return *k;
}

SDL_Surface* load_bmp(char* fn)
{
	SDL_Surface* s;
	char buff[32];
	#ifdef _WIN32
		strcpy(&buff[0], "win/");
	#elif _PSP
		strcpy(&buff[0], "psp/");
	#endif
	strcpy(&buff[_strlen(buff)], fn);
	s = SDL_LoadBMP(buff);
	if (!s)
		return NULL;
	else
		return s;
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

SDL_Surface* scale_surface(SDL_Surface *Surface, Uint16 Width, Uint16 Height)
{
	if (Surface->w == Width && Surface->h == Height)
		return Surface;

	else
	{
		if(!Surface || !Width || !Height)
			return 0;
     
		SDL_Surface *_ret = SDL_CreateRGBSurface(Surface->flags, Width, Height, Surface->format->BitsPerPixel,
			Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);
 
		double _stretch_factor_x = (static_cast<double>(Width)  / static_cast<double>(Surface->w)),
			_stretch_factor_y = (static_cast<double>(Height) / static_cast<double>(Surface->h));
 
		for(Sint32 y = 0; y < Surface->h; y++)
		{
			for(Sint32 x = 0; x < Surface->w; x++)
			{
				for(Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
				{
					for(Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
					{
						putpixel(_ret, static_cast<Sint32>(_stretch_factor_x * x) + o_x, 
							static_cast<Sint32>(_stretch_factor_y * y) + o_y, getpixel(Surface, x, y));
					   // DrawPixel(_ret, static_cast<Sint32>(_stretch_factor_x * x) + o_x, 
						 //   static_cast<Sint32>(_stretch_factor_y * y) + o_y, pixel(Surface, x, y));
					}
				}
			}
		}
 
		SDL_FreeSurface(Surface);
		Surface = NULL;
		return _ret;
	}
}

int str_end(char* s, int size)
{
	//run through whole string, find '<', leave..
	int id = -1, c = 0;
	for (; c < size; c++)
		id = s[c] == '>' ? c : id;
	return id;
}

int str_find(char* s, char c, int size)
{
	int id = -1, k = 0;
	for (; k < size; k++)
		id = s[k] == c ? k : id;
	return id;
}

//txt to break down <g><g>
font_surface* font_multicol_setup(TTF_Font* f, char* txt, SDL_Color b, SDL_Color h, int x, int y, bool center_text)
{
	font_surface* s;
	int i = 0, c = 0, surf_count = 0, str_size = str_end(txt, 64);
	//oversized int array to store color elems (if any)
	unsigned char* txt_col_ids = new unsigned char[str_size];
	int txt_col_count = 0;
	for (;c < str_size;) //null 0 last elem
	{
		//start of a colour code for the next word
		if (txt[c] == '<')
		{
			//increment to next id (log the col)
			txt_col_ids[txt_col_count] = txt[c+1];
			//increment to next id (log the txt array index)
			txt_col_ids[txt_col_count+1] = c;
			txt_col_count+=2;
			//divisable by 4 means we've hit this condition twice (which is the pre/post text color)
			if ((txt_col_count % 4) == 0)
				surf_count += 1;
			//move past '<' 'COL' '>' onto the coloured word
			c+=3;
		}

		else
			c++;
	}

	i = c = 0;
	//create the surfaces
	s = new font_surface[surf_count];
	for (; i < surf_count; i++)
	{
		int txt_split_count = (int)txt_col_ids[c+3] - (int)txt_col_ids[c+1];
		strncpy(&str[0], &txt[(int)txt_col_ids[c+1] + 3], txt_split_count - 3);
		str[txt_split_count - 3] = 0;
		switch(txt_col_ids[c])
		{
			case 'w':
				s[i].s = TTF_RenderText_Solid(f, str, w);
				break;
			case 'r':
				s[i].s = TTF_RenderText_Solid(f, str, r);
				break;
			case 'g':
				s[i].s = TTF_RenderText_Solid(f, str, g);
				break;
			case 'b':
				s[i].s = TTF_RenderText_Solid(f, str, b);
				break;
		}

		//if its the origin text, set it to the source
		if (i == 0)
		{
			s[i].r.x = x;
			s[i].r.y = y;
			if (center_text){
				s[i].r.x -= (s[i].s->w / 2);
				s[i].r.y -= (s[i].s->h / 2);
			}
		}

		else
		{
			//offset previous surface against the new one
			if (center_text){
				s[i-1].r.x -= (s[i].s->w / 2);
			}
			//create the whitespace between characters
			//change 8 to font whitespace pixel width
			s[i].r.x = s[i-1].r.x + s[i-1].s->w + 5;
			s[i].r.y = s[i-1].r.y;	
		}

		c+=4;
	}

	delete txt_col_ids;
	txt_col_ids = NULL;
	return s;
}

//n = number of font colours
void font_multicol_render(SDL_Surface* scr, font_surface* s, int n)
{
	int i;
	for (i = 0; i < n; i++){
		SDL_BlitSurface(s[i].s, NULL, scr, &s[i].r);
	}
}

void font_multicol_destroy(font_surface* s, int n)
{
	int i;
	for (i = 0; i < n; i++){
		SDL_FreeSurface(s[i].s);
	}
}