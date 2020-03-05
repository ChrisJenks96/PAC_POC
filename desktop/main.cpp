#include "core.h"

int main(int argc, char** argv)
{	
	core main_core;
	main_core.init(60);

	while (!main_core.is_quit())
	{
		main_core.pre_update();
		main_core.update();
		main_core.render();
	}
	
	return 0;
}