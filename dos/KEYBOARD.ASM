section .text

get_key:
	xor ax, ax ;0 ax reg
	mov ax, 073h ; 115 [s] key
	int 16h ;keyboard read interrupt

section .data ;put data here
	
section .bss ;put uninit data here