#include "core.h"

bool core::init(int _fps)
{
	fps = _fps;
	#ifdef _PSP
		/* Setup Homebutton Callbacks */
		SetupCallbacks();
		//setup analog stick input for PSP
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	#endif

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	screen = SDL_SetVideoMode(SCR_WIDTH, SCR_HEIGHT, SCR_BPP, SCR_SURF_MODE);
	SDL_WM_SetCaption("Who's a Good Boy!", NULL);

	if(!main_font.init(SCR_WIDTH, SCR_HEIGHT))
		return false;

	//init sdl mixer
	//Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	return true;
}

void core::pre_update()
{
	start_clock = SDL_GetTicks();
	//running is the func inside callbacks
	#ifdef _PSP
		quit = !running();
	#endif

	//refresh if the screen doesnt match tex size
	SDL_FillRect(screen, NULL, 0x000000);
	
}

void core::update()
{
	main_font.update();
	in.update();
}

void core::render()
{
	unsigned int timerFps = SDL_GetTicks() - start_clock; //I get the time it took to update and draw;
	if(timerFps < (1000.0f / (float)fps)) // if timerFps is < 16.6666...7 ms (meaning it loaded the frame too fast)
		SDL_Delay((1000.0f / (float)fps) - timerFps); //delay the frame to be in time	

	SDL_Flip(screen);
	deltatime = ((float)(SDL_GetTicks() - start_clock)) / 1000.0f;
	start_clock = SDL_GetTicks();
}

core::~core()
{
	SDL_Quit();
	//Quit SDL_ttf
    TTF_Quit();

	#ifdef _PSP
		sceKernelExitGame();
	#endif
}