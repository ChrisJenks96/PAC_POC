#ifdef _WIN32
	#include <SDL.h>
	#include <SDL_ttf.h>
	#include <SDL_mixer.h>

	#define SCR_WIDTH 800
	#define SCR_HEIGHT 600
	#define SCR_BPP 24
	#define SCR_SURF_MODE SDL_SWSURFACE

	#define FONT_SIZE 20
#else
	#include <pspkernel.h>
	#include <pspgu.h>
	#include <pspdebug.h>
	#include <pspdisplay.h>
	#include <pspctrl.h>
	#include "callbacks.h"
	#include <SDL/SDL.h>
	#include <SDL/SDL_ttf.h>
	#include <SDL/SDL_mixer.h>

	#define SCR_WIDTH 480
	#define SCR_HEIGHT 272
	#define SCR_BPP 16
	#define SCR_SURF_MODE SDL_SWSURFACE

	#define FONT_SIZE 14

	#define printf pspDebugScreenPrintf

	PSP_MODULE_INFO("PAC_POC", 0, 1, 1);
	PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
	PSP_HEAP_SIZE_KB(20480);
#endif

#include "main_menu.h"
#include "inventory.h"
#include "util.h"
#include "anim.h"
#include "events.h"

#include "video.h"

//main shit
bool music_playing_flag = false;
Mix_Music* music = NULL;
//tracking if the last played music is the same... hence we dont need to delete
int music_event_id = -1, music_event_id2 = -1;

int skip_cutscenes = 0;
int game_state = 1;
bool game_state_setup_flag = false;
TTF_Font* font = NULL;
SDL_Surface* scr = NULL;
SDL_Event e;
bool quit;
int startclock = 0;
float delta_time = 0.0f;
bool mouse_down = false;
//prevents us from skipping the first scene when going from
//main menu to the game (mouse down/up event hack)
bool menu_game_flag = false;

//used to show which background is shown
int bkg_id = BKG_0_ID;
int bkg_old_id = BKG_0_ID;
int bkg_size = BKG_0_SIZE;
int bkg_old_size = BKG_0_SIZE;
SDL_Surface* bkg, *cursor, *test;
font_surface* event_text = NULL;
int event_text_len = 0;
//number of cols in text
int event_text_col_number = 0;
//test... change me
event_seq* e_s = NULL;

//game specific code
/*#define SCOOTER_EVENTS_NUM 1
#define SCOOTER_EVENTS_FILE_NAME "SCOOT_EVENTS"
#define SCOOTER_ANIM_POS_FILE_NAME "SCOOT_ANIM"
#define SCOOTER_FILE_NAME "SCOOT_.bmp"
//#define SCOOTER_SPRITE_SIZE 256
#define SCOOTER_SPRITE_WIDTH_NUM 4
#define SCOOTER_FRAME_COUNT 40

anim_pos v[SCOOTER_FRAME_COUNT+2]; //+2 for start and end
//scoot animation
int scoot_anim_counter = 0;
int scoot_count = 0;
SDL_Surface* scoot_anim;
SDL_Rect scoot_anim_source;
SDL_Rect scoot_anim_dest;
bool scoot_anim_target = false;
int scoot_tmp_x, scoot_tmp_y;*/

static void sys_init()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	scr = SDL_SetVideoMode(SCR_WIDTH, SCR_HEIGHT, SCR_BPP, SCR_SURF_MODE);
	SDL_ShowCursor(0);
	SDL_WM_SetCaption("PAC_POC", NULL);
}

static bool main_menu_state_setup()
{
	//setup main menu
	main_menu_setup();
	return true;
}

static bool general_state_setup()
{
	#ifdef _PSP
		/* Setup Homebutton Callbacks */
		SetupCallbacks();
		//setup analog stick input for PSP
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	#endif

	//setup video modes for SDL
	sys_init();

	//setup font
	int err = TTF_Init();
	if (err == -1)
		return false;
	font = TTF_OpenFont("Chapaza.ttf", FONT_SIZE);
	if (font == NULL)
		return false;

	//init sdl mixer
	err = Mix_Init(MIX_INIT_OGG);
	if (err == -1)
		return false;
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);

	cursor = SDL_LoadBMP("cursor.bmp");
	SDL_SetColorKey(cursor, SDL_SRCCOLORKEY, SDL_MapRGB(cursor->format, 36, 149, 180));
	cursor_dest.h = cursor->h;
	cursor_dest.w = cursor->w;
	quit = false;
	return true;
}

static void main_menu_state_update()
{
	main_menu_render(font, scr, FONT_SIZE);
}

static bool game_state_setup()
{
	//SDL_SetColorKey(fore, SDL_RLEACCEL | SDL_SRCCOLORKEY, SDL_MapRGB(fore->format, 0, 0, 0));
	bkg = SDL_LoadBMP("main_camera_1.bmp");
	test = SDL_LoadBMP("test.bmp");

	//load event data in for current frame

	e_s = events_pos_parse("scenes/scene_1.events", SCR_WIDTH, SCR_HEIGHT);

	//load scooter animation in
	//scoot_anim = SDL_LoadBMP(SCOOTER_FILE_NAME);
	//SDL_SetColorKey(scoot_anim, SDL_RLEACCEL | SDL_SRCCOLORKEY, SDL_MapRGB(scoot_anim->format, 255, 255, 255));

	//load animation data in
	//anim_parse_pos(SCOOTER_ANIM_POS_FILE_NAME, v, SCR_WIDTH, SCR_HEIGHT);

	//breaking spr map into sub spr
	/*scoot_anim_source.h = v[0].size.y;
	scoot_anim_source.w = v[0].size.x;
	scoot_anim_source.x = 0;
	scoot_anim_source.y = 0;
	//rendering destination spr
	scoot_anim_dest.h = v[0].size.y;
	scoot_anim_dest.w = v[0].size.x;
	//set the animation strart frame
	//683 | 310
	scoot_anim_dest.x = v[0].xy.x;
	scoot_anim_dest.y = v[0].xy.y;*/

	//setup mock inventory sprites 
	inventory_init("inventory.bmp");
	return true;
}

static void game_state_update()
{
	if (!is_inventory)
	{
		//running through the scooter animation, will tidy up when finished...
		/*if (scoot_anim_counter < ((SCOOTER_FRAME_COUNT + 2) - 1))
		{
			bool b_a = true, b_b = true;
			if (!scoot_anim_target)
			{
				scoot_tmp_x = v[scoot_anim_counter+1].xy.x - v[scoot_anim_counter].xy.x;
				scoot_tmp_y = v[scoot_anim_counter+1].xy.y - v[scoot_anim_counter].xy.y;
				scoot_anim_dest.w = v[scoot_anim_counter].size.x;
				scoot_anim_dest.h = v[scoot_anim_counter].size.y;
				scoot_anim_source.x += v[scoot_anim_counter].size.x;
				if (scoot_anim_source.x >= (v[scoot_anim_counter].size.x * (SCOOTER_SPRITE_WIDTH_NUM - 1))){
					scoot_anim_source.x = 0;
					scoot_anim_source.y += v[scoot_anim_counter].size.y;
				}
				scoot_anim_target = true;
			}

			if (scoot_anim_dest.x >= v[scoot_anim_counter+1].xy.x)
				scoot_anim_dest.x += scoot_tmp_x * delta_time;
			else
				b_a = false;

			if (scoot_anim_dest.y >= v[scoot_anim_counter+1].xy.y)
				scoot_anim_dest.y += scoot_tmp_y * delta_time;
			else
				b_b = false;

			if (!b_a && !b_b)
			{
				scoot_anim_target = false;
				//reenforce rule for the last frame
				scoot_anim_counter++;
			}
		}*/

		//refresh screen not need with bkg
		//SDL_FillRect(scr, NULL, 0x00000000);

		//game stuff...
		SDL_BlitSurface(bkg, NULL, scr, NULL);
		
		//test stuff with boundaries
		//SDL_Rect r = {e_s->es1[1].pos_x, e_s->es1[1].pos_y, e_s->es1[1].size_x, e_s->es1[1].size_y};
		//SDL_BlitSurface(test, NULL, scr, &r);
		
		//-1 bug.. prevents out of bounds
		/*SDL_Rect r_pos = scoot_anim_dest;
		//adapt sprite to unity perspective 
		r_pos.x -= v[scoot_anim_counter].size.x / 2;
		r_pos.y -= v[scoot_anim_counter].size.y / 2;
		SDL_BlitSurface(scoot_anim, &scoot_anim_source, scr, &r_pos);
		SDL_BlitSurface(fore, NULL, scr, NULL);*/
			
	}

	if (inv_key_press)
		is_inventory = timed_bool(&is_inventory, &inv_key_press, delta_time, 0.5f);

	inventory_render(scr);
}

static void main_menu_state_destroy()
{
	SDL_FreeSurface(bkg);
	//SDL_FreeSurface(fore);
	main_menu_destroy();
}

static void game_state_destroy()
{
	inventory_destroy();
	events_pos_destroy(e_s);
	font_multicol_destroy(event_text, event_text_col_number);
	//SDL_FreeSurface(scoot_anim);
	if (music != NULL){
		Mix_FreeMusic(music);
		music = NULL;
	}
}

//when we click on a game hitbox, we update everything
static bool update_game_hitbox(int i, int j, bool go_back)
{
	//prevents it from being rendered if its equal to 0
	event_text_len = 0;

	//reset if all the events being done
	int last_id = e_s->es1[bkg_id + i].sub_events - 1;
	if (e_s->es1[bkg_id + i].es2[last_id].done)
	{
		int k = 0;
		for (; k < (last_id + 1); k++){
			e_s->es1[bkg_id + i].es2[k].done = false;
		}
	}

	if (!e_s->es1[bkg_id + i].es2[j].done)
	{
		//make sure the event is now done so we can move on to the next one
		e_s->es1[bkg_id + i].es2[j].done = true;
		if (e_s->es1[bkg_id + i].es2[j].id == NEXT_SCENE_ID)
		{
			SDL_FreeSurface(bkg);
			//the first shot of the game is not part of the script (05/01/2020)
			if ((bkg_id + i) == 0 && go_back)
				bkg = SDL_LoadBMP("main_camera_1.bmp");
			else
				bkg = SDL_LoadBMP(e_s->es1[bkg_id + i].es2[j].id_str);

			if (!go_back)
			{
				switch (bkg_id)
				{
					case BKG_0_ID:
					{
						bkg_old_id = bkg_id;
						bkg_old_size = bkg_size;
						bkg_id = BKG_1_ID; 
						bkg_size = BKG_1_SIZE; 
						break;
					}

					case BKG_1_ID:
					{
						bkg_old_id = bkg_id;
						bkg_old_size = bkg_size;
						if (i == 0){
							bkg_id = BKG_3_ID; 
							bkg_size = BKG_3_SIZE; 
						}
						else if (i == 1){
							bkg_id = BKG_2_ID; 
							bkg_size = BKG_2_SIZE; 
						}
						break;
					}

					case BKG_2_ID:
					{
						bkg_old_id = bkg_id;
						bkg_old_size = bkg_size;
						bkg_id = BKG_3_ID; 
						bkg_size = BKG_3_SIZE; 
						break;
					}

					case BKG_3_ID:
					{
						//bkg_old_id = bkg_id;
						//bkg_old_size = bkg_size;
						//bkg_id = BKG_4_ID; 
						//bkg_size = BKG_4_SIZE; 
						break;
					}
				}
			}

			SDL_Delay(1000);
			return true;
		}

		else if (e_s->es1[bkg_id + i].es2[j].id == TEXT_ID)
		{
			if (event_text)
				font_multicol_destroy(event_text, 1);
			event_text_len = str_end(e_s->es1[bkg_id + i].es2[j].id_str, 64);
			event_text_col_number = e_s->es1[bkg_id + i].es2[j].str_num_cols;
			event_text = font_multicol_setup(font, 
				e_s->es1[bkg_id + i].es2[j].id_str, w, r, (SCR_WIDTH / 2), SCR_HEIGHT - 30, true);
			return true;
		}

		else if (e_s->es1[bkg_id + i].es2[j].id == SOUND_ID)
		{
			//prevent us from reloading music if we're simply repeating it
			if (music_event_id != (bkg_id + i) && music_event_id2 != j)
			{
				//track our music event ids
				music_event_id = (bkg_id + i);
				music_event_id2 = j;

				if (music != NULL){
					Mix_FreeMusic(music);
					music = NULL;
				}
				else
				{
					music = Mix_LoadMUS(e_s->es1[bkg_id + i].es2[j].id_str);
					if (music == NULL){
						printf("%s", Mix_GetError());
						return false;
					}
				}
			}

			Mix_PlayMusic(music, false);
			music_playing_flag = true;
			return true;
		}

		else if (e_s->es1[bkg_id + i].es2[j].id == VIDEO_ID)
		{
			video_play2(scr, e_s->es1[bkg_id + i].es2[j].id_str, &sys_init);
			return true;
		}

	}

	else
		return false;

	return false;
}

int main(int argc, char** argv)
{	
	if (!general_state_setup())
		return -1;
	//we are only ever gonna be loading into the main menu first...
	if (game_state == 0)
		if (!main_menu_state_setup())
			return -1;

	while (!quit)
	{
		startclock = SDL_GetTicks();
		mouse_update(delta_time);
		pc_mouse_debug_coord(delta_time);

		#ifdef _PSP
			quit = !running();
		#endif

		//play music
		//If there is no music playing
		if (music_playing_flag)
		{
			//dont free music up here... done in the event system instead
			if(Mix_PlayingMusic() == 0 ){
				music_playing_flag = false;
			}
		}

		//events
		#ifdef _WIN32
			SDL_PollEvent(&e);
			switch (e.type)
			{
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_MOUSEBUTTONDOWN:
					{
						mouse_down = true;
						//if we start the game, switch to game game state
						game_state = font_select_id == 0 ? 1 : 0;

						//have we pressed the quit button on the main menu?
						quit = font_select_id == 2 ? true : false; //make this main menu state only
						break;
					}
				case SDL_MOUSEBUTTONUP:
					{
						if (mouse_down)
						{
							//check on the hitbox events
							if (game_state == 1)
							{
								int i = 0, j = 0;
								bool go_back_flag = false;
								//if we click at the bottom of the screen, we go backwards
								/*if (mx >= 0 && mx <= SCR_WIDTH && 
									my >= (SCR_HEIGHT - 20) && my <= SCR_HEIGHT)
								{
									//reset to old screen settings
									bkg_id = bkg_old_id;
									bkg_size = bkg_old_size;
									if (update_game_hitbox(i, j, true))
										go_back_flag = true;
								}*/

								if (!go_back_flag && menu_game_flag)
								{
									//go through the bkg's hitboxes
									for (; i < bkg_size; i++)
									{
										if (events_pos_update(&e_s->es1[bkg_id + i], mx, my))
										{
											//if we find it... run the sub events
											for (; j < e_s->es1[bkg_id + i].sub_events; j++){
												if (update_game_hitbox(i, j, false)){
													break;
												}
											}
										}
									}
								}

								else
									menu_game_flag = true;
							}
						}

						mouse_down = false;
						break;
					}
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym )
					{
						//toggle the inventory (in game)
						case SDLK_TAB:
							if (!inv_key_press)
								inv_key_press = true;
						default:
							break;
					}
			}
		#else
			sceCtrlReadBufferPositive(&pad, 1); 
			if (pad.Buttons != 0)
			{
				if (pad.Buttons & PSP_CTRL_SELECT)
				{
					if (!inv_key_press)
						inv_key_press = true;
				}

				if (pad.Buttons & PSP_CTRL_CROSS){
					//if we start the game, switch to game game state
					game_state = font_select_id == 0 ? 1 : 0;
				}
			}
		#endif

		if (game_state == 0)
			main_menu_state_update();
		if (game_state == 1)
		{
			if (!game_state_setup_flag)
			{
				//unload main menu
				main_menu_state_destroy();
				if (!game_state_setup())
					return -1;
				game_state_setup_flag = true;
				//for debugging
				if (!skip_cutscenes){
					video_play2(scr, "test", &sys_init);
				}

				SDL_Delay(1000);
			}

			game_state_update();
		}

		//always render cursor last
		if (event_text_len != 0)
			font_multicol_render(scr, event_text, event_text_col_number);
		SDL_BlitSurface(cursor, NULL, scr, &cursor_dest);

		SDL_Flip(scr);

		delta_time = ((float)(SDL_GetTicks() - startclock)) / 1000.0f;
		startclock = SDL_GetTicks();
	}

	game_state_destroy();

	SDL_FreeSurface(cursor);
	//Close the font that was used
    TTF_CloseFont(font);
	//Quit SDL_ttf
    TTF_Quit();
	//quit the sdl mixer
	Mix_Quit();
	SDL_Quit();

	#ifdef _PSP
		sceKernelExitGame();
	#endif

	return 0;
}