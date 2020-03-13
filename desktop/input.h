#ifndef INPUT_H
#define INPUT_H

#ifdef _WIN32
	#include <SDL.h>
#else
	#include <SDL/SDL.h>
#endif

#include <memory.h>

class input
{
	public:
		input(){quit = false; memset(&keys[0], 0, 256);}
		void update();
		bool* get_keys(){return keys;}
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
		bool keys[256];
		bool quit;
		SDL_Event e;
};

#endif
