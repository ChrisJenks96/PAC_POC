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
			mov l, esi //return count
		}
	#else 
		l = strlen(s);
	#endif
	return l;
}

#endif