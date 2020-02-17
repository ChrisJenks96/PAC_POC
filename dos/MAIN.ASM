;https://thestarman.pcministry.com/asm/debug/8086REGs.htm#REGS
;https://everything2.com/title/VGA+Mode+13h

section .text
org 100h
global _start
_start: ;put code here
	jmp load_bmp_open_file

vga_main:
	mov ax, 0013h ;VGA mode
	int 10h ; 320x200x256col

	mov ax, 0A000h ;video memory address SVGA
	mov es, ax ;ES register now points to video memory
	mov si, bmp_pixels

	jmp put_pixel

pixel_new_row: ;used in put pixel to increment y
	mov bx, [orig_x] ;set x to 0
	mov [x], bx ;write the reg back to the var x

	mov ax, [y] ;move y back into ax again, will have been overwritten during pixel placement
	mov cx, [orig_y]
	add cx, 15
	cmp ax, cx ;if its equal to 15 (last row), end
	je device_end

	inc ax ;plus 1 to the y
	mov [y], ax

	jmp put_pixel ;move back to the put pixel function

put_pixel:
	;FIRST PIXEL 4 BITS
	mov  ax, 320
	imul  ax, [y]
	add  ax, [x]
	mov  bx, ax ;320*y+x into bx reg
	mov  al, [si] ;move curr col byte into al
	;e.g. red 00CC -> AH [0000 0000] AL [1101 1101]
	shr al, 04h ;-> AH [0000 0000] AL [0000 1101] 
	mov  [es:bx], al

	mov bx, [x] ;x
	mov cx, [orig_x]
	add cx, 15
	cmp bx, cx ;if x is 3 pixels wide, change y
	je pixel_new_row

	inc bx ;add 1 to x 
	mov [x], bx ;write the reg back to the var x

	;NEXT PIXEL 4 BITS
	mov  ax, 320
	imul  ax, [y]
	add  ax, [x]
	mov  bx, ax ;320*y+x into bx reg
	mov  ah, [si] ;move curr col byte into ah
	;e.g. red 00CC -> AH [1101 1101] AL [0000 0000]
	shr ah, 04h ;-> AH [0000 1101] AL [1101 0000] 
	mov  [es:bx], ah

	mov bx, [x] ;x
	mov cx, [orig_x]
	add cx, 15
	cmp bx, cx ;if x is 3 pixels wide, change y
	je pixel_new_row

	inc bx ;add 1 to x 
	mov [x], bx ;write the reg back to the var x

	inc si

	jmp put_pixel

device_end:
	ret

section .data ;put data here
	x dd 46 ;x and orig_x ,y and orig_y have to be the same
	y dd 46
	orig_x dd 46
	orig_y dd 46

section .bss ;put uninit data here

%include 'BITMAP.ASM'