all:
	g++ -g -c bin_image.cpp
	g++ -g -c core.cpp
	g++ -g -c enemy.cpp
	g++ -g -c font.cpp
	g++ -g -c input.cpp
	g++ -g -c main.cpp
	g++ -g -c physics_sprite.cpp
	g++ -g -c sound.cpp
	g++ -g -c sprite.cpp
	g++ -o main -w bin_image.o core.o enemy.o font.o input.o main.o physics_sprite.o sound.o sprite.o -lSDL -lSDL_ttf

clean:
	rm bin_image.o core.o enemy.o font.o input.o main.o physics_sprite.o sound.o sprite.o main
