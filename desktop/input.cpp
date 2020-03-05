#include "input.h"

void input::update()
{
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
					default:
						break;
				}
			case SDL_KEYUP:
				switch(e.key.keysym.sym )
				{
					default:
						break;
				}
			case SDL_KEYDOWN:
				switch(e.key.keysym.sym )
				{
					default:
						break;
				}
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