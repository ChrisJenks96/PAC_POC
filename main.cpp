#ifdef _WIN32
	#include <SDL.h>
	#include <SDL_ttf.h>
	#include <SDL_mixer.h>

	#define SCR_WIDTH 800
	#define SCR_HEIGHT 600
	#define SCR_BPP 32
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
	#define SCR_BPP 32
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
#include "save.h"
#include "video.h"

//main shit
bool music_playing_flag = false;
Mix_Music* music = NULL;
//tracking if the last played music is the same... hence we dont need to delete
int music_event_id = -1, music_event_id2 = -1;
//used to activate a save game once (event based key press)
bool save_game_flag = false;
bool game_state_setup_flag = false;
TTF_Font* font = NULL;
SDL_Surface* scr = NULL;
SDL_Event e;
bool quit = false;
int startclock = 0;
float delta_time = 0.0f;
bool mouse_down = false;
//prevents us from skipping the first scene when going from
//main menu to the game (mouse down/up event hack)
bool menu_game_flag = false;

//used to show which background is shown
int bkg_id = BKG_0_ID;
int bkg_id_offset = 0;
int bkg_sub_event_id = 0;
int bkg_old_id = BKG_0_ID;
//used for going backwards to the last frame... currently W.I.P (14/01/2020)
int bkg_size = BKG_0_SIZE;
int bkg_old_size = BKG_0_SIZE;
//the final dest of the bkg surface
SDL_Rect bkg_dest;
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

	//remove the screen if its passed the first time
	if (scr == NULL)
	{
		scr = SDL_SetVideoMode(SCR_WIDTH, SCR_HEIGHT, SCR_BPP, SCR_SURF_MODE);
		SDL_ShowCursor(0);
		SDL_WM_SetCaption("PAC_POC", NULL);
	}
}

static bool main_menu_state_setup()
{
	//setup main menu
	main_menu_setup(scr);
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

	cursor = load_bmp("cursor.bmp");
	SDL_SetColorKey(cursor, SDL_SRCCOLORKEY, SDL_MapRGB(cursor->format, 36, 149, 180));
	cursor_dest.h = cursor->h;
	cursor_dest.w = cursor->w;
	return true;
}

static void main_menu_state_update()
{
	main_menu_render(font, scr, FONT_SIZE);
}

static bool game_state_setup()
{
	//depending on the main menu selection
	if (font_select_id == 0)
		game_start_state = GS_NEW_GAME;
	else
	{
		game_start_state = GS_LOAD_GAME;
		//load the game, if no save exists, assign the new game flag again else continue on
		if (!game_load()){
			//start new game mode
			game_start_state = GS_NEW_GAME;
		}
	}

	if (game_start_state == GS_NEW_GAME){
		//load event data in for current frame
		e_s = events_pos_parse("scenes/scene_1.events", SCR_WIDTH, SCR_HEIGHT);
		if (e_s == NULL)
			return false;
		bkg = scale_surface(load_bmp("main_camera_1.bmp"), SCR_WIDTH, SCR_HEIGHT);
	}

	else if (game_start_state == GS_LOAD_GAME)
	{
		//sync the loaded save variables with current game
		bkg_id = gs_bkg_id + 1;
		bkg_size = gs_bkg_size;
		bkg_id_offset = gs_bkg_id_offset;

		//load event data in for current frame
		e_s = events_pos_parse(scn, SCR_WIDTH, SCR_HEIGHT);
		if (e_s == NULL)
			return false;
		bkg = scale_surface(load_bmp(e_s->es1[gs_bkg_id].es2[0].id_str), SCR_WIDTH, SCR_HEIGHT);
	}

	//offset it so we can centralise the bkg
	bkg_dest.x = (Sint16)((SCR_WIDTH - bkg->w) / 2);
	bkg_dest.y = (Sint16)((SCR_HEIGHT - bkg->h) / 2);

	//load scooter animation in
	//scoot_anim = load_bmp(SCOOTER_FILE_NAME);
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
		SDL_BlitSurface(bkg, NULL, scr, &bkg_dest);
		
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
static bool update_game_hitbox(bool go_back)
{
	//prevents it from being rendered if its equal to 0
	event_text_len = 0;

	//reset if all the events being done
	int last_id = e_s->es1[bkg_id + bkg_id_offset].sub_events - 1;
	if (e_s->es1[bkg_id + bkg_id_offset].es2[last_id].done)
	{
		int k = 0;
		for (; k < (last_id + 1); k++){
			e_s->es1[bkg_id + bkg_id_offset].es2[k].done = false;
		}
	}

	if (!e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].done)
	{
		//make sure the event is now done so we can move on to the next one
		e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].done = true;
		if (e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].id == NEXT_SCENE_ID)
		{
			SDL_FreeSurface(bkg);
			//the first shot of the game is not part of the script (05/01/2020)
			if ((bkg_id + bkg_id_offset) == 0 && go_back)
				bkg = scale_surface(load_bmp("main_camera_1.bmp"), SCR_WIDTH, SCR_HEIGHT);
			else
				bkg = scale_surface(load_bmp(e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].id_str), SCR_WIDTH, SCR_HEIGHT);

			//offset it so we can centralise the bkg
			bkg_dest.x += ((SCR_WIDTH - bkg->w) / 2);
			bkg_dest.y += ((SCR_HEIGHT - bkg->h) / 2);

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
						if (bkg_id_offset == 0){
							bkg_id = BKG_3_ID; 
							bkg_size = BKG_3_SIZE; 
						}
						else if (bkg_id_offset == 1){
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

			SDL_Delay(GAME_DEFAULT_DELAY);
			return true;
		}

		else if (e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].id == TEXT_ID)
		{
			if (event_text)
				font_multicol_destroy(event_text, 1);
			event_text_len = str_end(e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].id_str, 64);
			event_text_col_number = e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].str_num_cols;
			event_text = font_multicol_setup(font, 
				e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].id_str, w, r, (SCR_WIDTH / 2), SCR_HEIGHT - 30, true);
			return true;
		}

		else if (e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].id == SOUND_ID)
		{
			//prevent us from reloading music if we're simply repeating it
			if (music_event_id != (bkg_id + bkg_id_offset) && music_event_id2 != bkg_sub_event_id)
			{
				//track our music event ids
				music_event_id = (bkg_id + bkg_id_offset);
				music_event_id2 = bkg_sub_event_id;

				if (music != NULL){
					Mix_FreeMusic(music);
					music = NULL;
				}
				else
				{
					music = Mix_LoadMUS(e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].id_str);
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

		else if (e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].id == VIDEO_ID){
			video_play2(scr, e_s->es1[bkg_id + bkg_id_offset].es2[bkg_sub_event_id].id_str, &sys_init);
			return true;
		}

	}

	else
		return false;

	return false;
}

static void game_event_update()
{
	#ifdef _WIN32
	if (e.type == SDL_MOUSEBUTTONDOWN)
	#elif _PSP
	if (isKeyDown(PSP_CTRL_CROSS))
	#endif
	{
		mouse_down = true;
	}

	#ifdef _WIN32
	if (e.type == SDL_MOUSEBUTTONUP)
	#elif _PSP
	if (isKeyUp(PSP_CTRL_CROSS))
	#endif
	{
		if (mouse_down)
		{
			//check on the hitbox events
			if (game_state == 1)
			{
				bkg_id_offset = 0, bkg_sub_event_id = 0;
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
					bool found = false;
					//go through the bkg's hitboxes
					for (; bkg_id_offset < bkg_size; bkg_id_offset++)
					{
						if (events_pos_update(&e_s->es1[bkg_id + bkg_id_offset], mx, my))
						{
							//if we find it... run the sub events
							for (; bkg_sub_event_id < e_s->es1[bkg_id + bkg_id_offset].sub_events; bkg_sub_event_id++){
								if (update_game_hitbox(false)){
									found = true;
									break;
								}
							}
						}

						if (found)
							break;
					}
				}

				else
					menu_game_flag = true;
			}
		}
		
		mouse_down = false;
	}
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

		if (save_game_flag)
		{
			//set all game save variables before saving
			gs_scene_id = 0;
			//-1 because we start before the 0th bkg hence we need to jump back one frame
			//no bkg_id_offset as this prevents the background from being loaded in array elem 0
			gs_bkg_id = (bkg_id - 1);// + bkg_id_offset;
			gs_bkg_size = bkg_size;
			gs_bkg_id_offset = bkg_id_offset;
			game_save();
			save_game_flag = false;
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
						//if we start the game, switch to game game state
						game_state = font_select_id == 0 || font_select_id == 1 ? 1 : 0;
						//have we pressed the quit button on the main menu?
						quit = font_select_id == 2 ? true : false; //make this main menu state only
						break;
					}
				case SDL_KEYUP:
					switch(e.key.keysym.sym )
					{
						case SDLK_F1:
							save_game_flag = false;
							break;
					}
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym )
					{
						case SDLK_F1:
							save_game_flag = true;
							break;
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
			sceCtrlReadLatch(&latch);
			if (pad.Buttons != 0)
			{
				if (isKeyDown(PSP_CTRL_SELECT))
				{
					if (!inv_key_press)
						inv_key_press = true;
				}

				if (isKeyDown(PSP_CTRL_CROSS)){
					//if we start the game, switch to game game state
					game_state = font_select_id == 0 || font_select_id == 1 ? 1 : 0;
					//have we pressed the quit button on the main menu?
					quit = font_select_id == 2 ? true : false; //make this main menu state only
				}

				if (isKeyDown(PSP_CTRL_START))
					save_game_flag = true;
				if (isKeyUp(PSP_CTRL_START))
					save_game_flag = false;
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
					quit = true;
				game_state_setup_flag = true;
				//for debugging
				video_play2(scr, "test", &sys_init);
				SDL_Delay(GAME_DEFAULT_DELAY);
			}

			game_state_update();
		}

		game_event_update();

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