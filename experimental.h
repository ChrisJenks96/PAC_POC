#ifndef EXPERIMENTAL_H
#define EXPERIMENTAL_H

#include <stdio.h>
#include <stdint.h>

//example layout
static inline uint16_t _increment(uint16_t n, uint16_t n2)
{
	#ifdef _WIN32
		uint16_t data;
		__asm
		{
			mov dx, n
			add dx, n2
			mov data, dx
		}
		return data;
	#else
		return 0;
	#endif
}

//https://stackoverflow.com/questions/25796257/trying-to-figure-out-masm-syntax
static inline int _strlen(const char* s)
{
	int l;

	#ifdef _WIN32
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
	#else 
		l = strlen(s);
	#endif
	return l;
}

static inline void _test1()
{
	_asm
	{
		//adventurous (not fastest) way of going from 15 to 9
		mov AL, 00Fh //0000 1111
		shr AL, 03h //0000 0001
		shl AL, 03h //0000 1000
		add AL, 07h //0000 1111
		xor AL, 009h //0000 1111 15 1001 9 0110 6 
		or AL, 0F0h //flip the first 4 bits for not to flip them back or make the line above xor AL, 0F9h
		not AL //0110 1001 9
	}
	return;
}

#endif