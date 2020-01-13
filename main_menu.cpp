#include "main_menu.h"

int font_select_id;
SDL_Color font_c_highlight = {255,255,255};
SDL_Color font_c_unhighlight = {128,128,128};
SDL_Rect r1;
SDL_Surface* s;

SDL_Surface* grad_bkg;

void main_menu_setup()
{
	grad_bkg = SDL_LoadBMP("mm_bkg.bmp");
}

bool mm_font_setup_surface(TTF_Font* f, SDL_Surface* scr, int font_size, char* txt, int y_offset)
{
	//prelim to get the surface width (could probably do this without having to get the font)
	s = TTF_RenderText_Solid(f, txt, font_c_highlight);
	r1.x = (scr->w - s->w) / 2;
	r1.y = ((scr->h / 2) - s->h) - y_offset;
	
	int nw = (cursor_dest.w / 2);
	int nh = (cursor_dest.h / 2);
	if (mx >= (r1.x - nw) && mx <= ((r1.x + s->w) + nw) && 
		my >= (r1.y - nh) && my <= ((r1.y + s->h))) 
	{
		SDL_FreeSurface(s);
		s = TTF_RenderText_Solid(f, txt, font_c_highlight);
		return true;
	}

	else
	{
		SDL_FreeSurface(s);
		s = TTF_RenderText_Solid(f, txt, font_c_unhighlight);
		return false;
	}
}

void main_menu_render(TTF_Font* f, SDL_Surface* scr, int font_size)
{
	font_select_id = -1;

	//render bkg
	SDL_BlitSurface(grad_bkg, NULL, scr, NULL);

	//render fonts 	
	font_select_id = mm_font_setup_surface(f, scr, font_size, "New Game", 50) == 1 ? 0 : font_select_id;
	SDL_BlitSurface(s, NULL, scr, &r1);
	SDL_FreeSurface(s);

	font_select_id = mm_font_setup_surface(f, scr, font_size, "Load Saved Game", 0) == 1 ? 1 : font_select_id;
	SDL_BlitSurface(s, NULL, scr, &r1);
	SDL_FreeSurface(s);

	font_select_id = mm_font_setup_surface(f, scr, font_size, "Exit Game", -50) == 1 ? 2 : font_select_id;
	SDL_BlitSurface(s, NULL, scr, &r1);
	SDL_FreeSurface(s);
}

void main_menu_destroy()
{
	SDL_FreeSurface(grad_bkg);
	grad_bkg = NULL;
}