#ifndef INPUT_H
#define INPUT_H

#ifdef _WIN32
	#include <SDL.h>
#else
	#include <SDL/SDL.h>
#endif

class input
{
	public:
		input(){quit = false;}
		void update();
		bool is_quit(){return quit;}
		#ifdef _PSP
			int isKeyDown(int key);
			int isKeyUp(int key);
		#endif
		~input();
	private:
		#ifdef _PSP
			SceCtrlData pad;
			SceCtrlLatch latch;
		#endif
		bool quit;
		SDL_Event e;
};

#endif
