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
	#elif _PSP
		ratio = (float)width / 480.0f;
		size = (int)(14.0f * ratio);
	#endif

	mf = TTF_OpenFont("Chapaza.ttf", size);
	if (mf == NULL)
		return false;

	return true;
}

font_surface* font::create(TTF_Font* f, char* txt, SDL_Color b, SDL_Color h, int x, int y, bool center_text)
{
	font_surface* s;
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
	s = new font_surface[surf_count];
	for (; i < surf_count; i++)
	{
		int txt_split_count = (int)txt_col_ids[c+3] - (int)txt_col_ids[c+1];
		strncpy(&str[0], &txt[(int)txt_col_ids[c+1] + 3], txt_split_count - 3);
		str[txt_split_count - 3] = 0;
		switch(txt_col_ids[c])
		{
			case 'w':
				s[i].s = TTF_RenderText_Solid(f, str, white);
				break;
			case 'r':
				s[i].s = TTF_RenderText_Solid(f, str, red);
				break;
			case 'g':
				s[i].s = TTF_RenderText_Solid(f, str, green);
				break;
			case 'b':
				s[i].s = TTF_RenderText_Solid(f, str, blue);
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

void font::update()
{

}

void font::render(SDL_Surface* scr, font_surface* s, int n)
{
	int i;
	for (i = 0; i < n; i++){
		SDL_BlitSurface(s[i].s, NULL, scr, &s[i].r);
	}
}

void font::destroy(font_surface* s, int n)
{
	int i;
	for (i = 0; i < n; i++){
		SDL_FreeSurface(s[i].s);
	}
}

font::~font()
{
	//Close the font that was used
    TTF_CloseFont(mf);
	//Quit SDL_ttf
    TTF_Quit();
}