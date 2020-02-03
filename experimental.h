#ifndef EXPERIMENTAL_H
#define EXPERIMENTAL_H

#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
	#include <SDL.h>

#elif _PSP
	#include <SDL/SDL.h>
#endif

#define _ASM

//https://stackoverflow.com/questions/25796257/trying-to-figure-out-masm-syntax
static inline int _strlen(const char* s)
{
	int l;
	#ifdef _WIN32
	#ifdef _ASM
		_asm
		{
			mov eax, s //move address of the string into eax reg
			mov esi, -1 //set to -1

			_strlen_loop:
				inc esi //increment the esi reg count -1 .. 0 .. 
				mov ebx, [eax + esi] //move through the string pointer (next char)
				and ebx, 0FFh //GOOGLE THIS... (bitwise result not of 0)???
				jnz _strlen_loop //loop round again...

			mov eax, esi
			mov l, eax //return count
		}
	#endif
	#else
		l = strlen(s);
	#endif
	return l;
}

static inline int _str_find(char* s, char c)
{
	int l;
	#ifdef _WIN32
	#ifdef _ASM
		_asm
		{
			mov eax, s //mov str address into 32 bit reg
			dec eax //move back as we increment off the bat in the loop
			mov esi, -1 //set to -1
			mov BL, c //move comparing char into 8 bit BL
		
			_str_find_loop:
				inc esi
				inc eax //move forward on the string pointer
				mov BH, [eax] //move first elem str array into 8 bit BH
				cmp BL, BH //compare both 8 bit reg
				je _str_find_found //if its found, log the id and continue till end of str
				cmp BH, 00h //if we hit the end of the string (0 null term) time to quit
				je _str_find_end //jump to end game
				jmp _str_find_loop //rinse and repeat

			_str_find_found:
				mov l, esi //return count	
				jmp _str_find_loop

			_str_find_end:
		}
	#endif
	#else
		//clone of the utils.h version but no scope to call w/o recursion
		int k = 0;
		l = 0;
		while (true)
		{
			l = s[k] == c ? k : l;
			k++;
			if (s[k] == 0)
				break;
		}

	#endif
	return l;
}

static inline void _test1()
{
	#ifdef _WIN32
	#ifdef _ASM
		_asm
		{
			//adventurous (not fastest) way of going from 15 to 9
			mov AL, 00Fh //1000 1111
			shr AL, 03h //0000 0001
			shl AL, 03h //0000 1000
			add AL, 07h //0000 1111
			xor AL, 009h //0000 1111 15 1001 9 0110 6 
			or AL, 0F0h //flip the first 4 bits for not to flip them back or make the line above xor AL, 0F9h
			not AL //0110 1001 9
		}

		_asm
		{
			//128 to 5 fastest way
			mov AL, 080h //1000 0000 //0000 0101
			xor AL, 085h //1000 0101
		}
	#endif
	#endif
	return;
}


static int _SDL_UpperBlit (SDL_Surface *src, SDL_Rect *srcrect,
	SDL_Surface *dst, SDL_Rect *dstrect)
{
	int srcx, srcy, w, h;

	srcx = srcy = 0;
	w = src->w;
	h = src->h;

	int dx, dy;
	SDL_Rect *clip = &dst->clip_rect;
	dx = dstrect->x + w - clip->x - clip->w;
	dy = dstrect->y + h - clip->y - clip->h;

	SDL_Rect sr;
	sr.x = srcx;
	sr.y = srcy;
	sr.w = dstrect->w = w;
	sr.h = dstrect->h = h;
	return SDL_LowerBlit(src, &sr, dst, dstrect);
}

#endif