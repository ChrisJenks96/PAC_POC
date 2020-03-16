#include "font.h"

bool font::init(int width, int height)
{
	//setup font
	if (TTF_Init() == -1)
		return false;

	//font ratio changes with the width and height
	#ifdef _WIN32
		ratio = (float)width / 800.0f;
		size = (int)(20.0f * ratio);
	#elif __linux__
		ratio = (float)width / 800.0f;
		size = (int)(20.0f * ratio);
	#elif _PSP
		ratio = (float)width / 480.0f;
		size = (int)(14.0f * ratio);
	#endif

	mf = TTF_OpenFont("ASSETS/upheavtt.ttf", 16);
	if (mf == NULL)
		return false;

	return true;
}

void font::create(char* txt, int x, int y, bool center_text)
{
	int i = 0, c = 0, surf_count = 0, str_size = str_find(txt, '>');
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
	mfs = new font_surface[surf_count];
	for (; i < surf_count; i++)
	{
		int txt_split_count = (int)txt_col_ids[c+3] - (int)txt_col_ids[c+1];
		strncpy(&str[0], &txt[(int)txt_col_ids[c+1] + 3], txt_split_count - 3);
		str[txt_split_count - 3] = 0;
		switch(txt_col_ids[c])
		{
			case 'w':
				mfs[i].s = TTF_RenderText_Solid(mf, str, white);
				break;
			case 'r':
				mfs[i].s = TTF_RenderText_Solid(mf, str, red);
				break;
			case 'g':
				mfs[i].s = TTF_RenderText_Solid(mf, str, green);
				break;
			case 'b':
				mfs[i].s = TTF_RenderText_Solid(mf, str, blue);
				break;
			case 'y':
				mfs[i].s = TTF_RenderText_Solid(mf, str, yellow);
				break;
		}

		//if its the origin text, set it to the source
		if (i == 0)
		{
			mfs[i].r.x = x;
			mfs[i].r.y = y;
			if (center_text){
				mfs[i].r.x -= (mfs[i].s->w / 2);
				mfs[i].r.y -= (mfs[i].s->h / 2);
			}
		}

		else
		{
			//offset previous surface against the new one
			if (center_text){
				mfs[i-1].r.x -= (mfs[i].s->w / 2);
			}
			//create the whitespace between characters
			//change 8 to font whitespace pixel width
			mfs[i].r.x = mfs[i-1].r.x + mfs[i-1].s->w + 5;
			mfs[i].r.y = mfs[i-1].r.y;	
		}

		c+=4;
	}

	delete txt_col_ids;
	txt_col_ids = NULL;
}

void font::update()
{

}

void font::render(SDL_Surface* scr, int n)
{
	int i;
	for (i = 0; i < n; i++){
		SDL_BlitSurface(mfs[i].s, NULL, scr, &mfs[i].r);
	}
}

void font::destroy(int n)
{
	int i;
	for (i = 0; i < n; i++){
		SDL_FreeSurface(mfs[i].s);
	}
}

font::~font()
{
	//Close the font that was used
    TTF_CloseFont(mf);
}