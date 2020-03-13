#include "input.h"

void input::update()
{
	//events
	#ifdef _WIN32
	#elif __linux__
		SDL_PollEvent(&e);
		switch (e.type)
		{
			case SDL_QUIT:
				quit = true;
				break;
			/*case SDL_MOUSEBUTTONDOWN:
				{
					default:
						break;
				}*/
			case SDL_KEYUP:
				keys[e.key.keysym.sym] = false;
				break;
			case SDL_KEYDOWN:
				keys[e.key.keysym.sym] = true;
				break;
		}
	#else
		sceCtrlReadBufferPositive(&pad, 1); 
		sceCtrlReadLatch(&latch);
		if (pad.Buttons != 0)
		{

		}
	#endif
}

#ifdef _PSP
	int input::isKeyDown(int key){
		return (latch.uiMake & key);
	}

	int input::isKeyUp(int key);{
		return (latch.uiBreak & key);
	}
#endif

input::~input()
{

}