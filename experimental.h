#ifndef EXPERIMENTAL_H
#define EXPERIMENTAL_H

#include <stdio.h>
#include <stdint.h>

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

#endif