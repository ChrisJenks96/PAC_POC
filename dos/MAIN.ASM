;https://thestarman.pcministry.com/asm/debug/8086REGs.htm#REGS
;https://everything2.com/title/VGA+Mode+13h

section .text
org 100h
global _start
_start: ;put code here
	jmp load_bmp_open_file

vga_main:
	mov ax, 0A000h ;video memory address SVGA
	mov es, ax ;ES register now points to video memory
	
	jmp init_update

init_update: ;call this before calling update loop, resets all vars
	;used as a clear screen
	mov ax, 0013h ;VGA mode
	int 10h ; 320x200x256col

	mov si, bmp_pixels ;reset the si pointer to start of the bmp pixel data
	mov ax, [x] ;reset x
	mov bx, [orig_x]
	mov ax, bx
	mov ax, [y] ;reset y
	mov bx, [orig_y]
	mov ax, bx

	jmp update

update: ;loop through all update/render funcs
	call get_key ;AX will hold the key we pressed...
	call move_player ;MUST CALL STRAIGHT AFTER 'get_key', move the player based on the ax value
	
	;put_pixel last to be drawn
	jmp put_pixel ;render out the sprite

pixel_new_row: ;used in put pixel to increment y
	mov bx, [orig_x] ;set x to 0
	mov [x], bx ;write the reg back to the var x

	mov ax, [y] ;move y back into ax again, will have been overwritten during pixel placement
	mov cx, [orig_y]
	add cx, 16
	cmp ax, cx ;if its equal to 15 (last row), end
	je init_update ;jump back to the start of the update loop again

	inc ax ;plus 1 to the y
	mov [y], ax

	mov ax, [put_pixel_func_flag]
	cmp ax, 01h
	je put_pixel ;if we came out of put_pixel(1), we want to continue to put_pixel2
	jmp put_pixel2 ;else move back to the put_pixel(1) function

put_pixel:
	;post pixel_new_row func, we need to know where to continue from pixel data wise
	;put_pixel (1) sets the flag to 0
	xor ax, ax  ;0 ax reg (xor ax, ax = smaller (2b vs 5b) and addition of clearing CF)
	mov [put_pixel_func_flag], ax 

	mov  ax, 320
	imul  ax, [y] ;320*y
	add ax, [x]
	inc ax ; x + 1
	mov  bx, ax ;320*y+x into bx reg
	;mov ax, 00h ; 0 off ax reg (will contain 320*y still, not needed)
	xor ax, ax ;0 ax reg (xor ax, ax = smaller (2b vs 5b) and addition of clearing CF)
	mov  al, [si] ;move curr col byte into al
	;e.g. red 00CC -> AH [0000 0000] AL [*1101* 1101]
	shr al, 04h ;-> AH [0000 0000] AL [0000 *1101*] 
	mov  [es:bx], al

	mov bx, [x] ;x 
	inc bx ;add 1 to x
	mov [x], bx ;write the reg back to the var x 
	mov cx, [orig_x] ;move the target x into the cx reg
	add cx, 16 ;append the 16px (15 = 16px - 1) onto the target to simulate the width of sprite
	cmp bx, cx ;check to see curr x is equal to target x
	je pixel_new_row ;if so, 0 the x and increment the y
	
	jmp put_pixel2

put_pixel2:
	;post pixel_new_row func, we need to know where to continue from pixel data wise
	;put_pixel2 sets the flag to 0
	mov ax, 01h
	mov [put_pixel_func_flag], ax

	mov ax, 320
	imul ax, [y]
	add ax, [x]
	inc ax ; x + 1
	mov bx, ax
	xor ax, ax ;0 ax reg (xor ax, ax = smaller (2b vs 5b) and addition of clearing CF)
	mov  al, [si] ;move curr col byte into al
	;e.g. red 00CC -> AH [0000 0000] AL [1101 *1101*]
	shl al, 04h ;-> AH [0000 0000] AL [*1101* 0000] 
	shr al, 04h ;-> AH [0000 0000] AL [0000 *1101*] 
	mov  [es:bx], al ;

	add si, 01h ;add 1 byte increment (8 bit shift, 2x 4 bit colours, hence we do 2x pixels in put_pixel)

	mov bx, [x]
	inc bx
	mov [x], bx
	mov cx, [orig_x]
	add cx, 16
	cmp bx, cx
	je pixel_new_row

	jmp put_pixel

device_end:
	ret

section .data ;put data here
	;for x & y (& orig_) set -1 of dest coords, we have to increment x off the bat in put_pixel for 2x checks
	x dw 20 ;x and orig_x ,y and orig_y have to be the same
	y dw 20
	orig_x dw 20
	orig_y dw 20
	put_pixel_func_flag db 0 ;don't modify! this track which put_pixel we return to after pixel_new_row

section .bss ;put uninit data here

%include 'BITMAP.ASM'
%include 'KEYBOARD.ASM'