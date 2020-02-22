;https://thestarman.pcministry.com/asm/debug/8086REGs.htm#REGS
;https://everything2.com/title/VGA+Mode+13h

section .text
org 100h
global _start
_start: ;put code here
	mov dx, player_fn ;file name to load
	call load_bmp_open_file ;load the file and pointer
	mov dx, player_spr ;the sprite we want to load the data into
	call load_bmp_data ;call the bitmap data loader from the file we've opened
	
	mov dx, platform_fn
	call load_bmp_open_file
	mov dx, platform_spr
	call load_bmp_data
	
	jmp vga_main

;8088 4.77 MHz 	315 cycles on DOSBOX
vga_main:
	bits 16
	;used as a clear screen
	mov ax, 0013h ;VGA mode
	int 10h ; 320x200x256col

	mov ax, 0A000h ;video memory address SVGA
	mov es, ax ;ES register now points to video memory
	jmp init_update

init_update: ;call this before calling update loop, resets all vars
	call waitforRetrace  ; destroys al, dx

	mov ax, [x] ;reset x
	mov bx, [orig_x]
	mov [x], bx
	mov ax, [y] ;reset y
	mov bx, [orig_y]
	mov [y], bx

	jmp update ;jump to update

update: ;loop through all update/render funcs
	call get_key ;AX will hold the key we pressed...
	call move_player_d ;MUST CALL STRAIGHT AFTER 'get_key', move the player based on the ax value
	call move_player_a
	call move_player_w
	call move_player_s

	mov si, player_spr ;reset the si pointer to start of the bmp pixel data

	mov cx, ;THIS WILL ERROR ON THIS LINE... INTENTIONAL, USE CX TO HOLD A POINTER TO AN ARRAY OF (X,Y,ORIG_X,ORIG_Y) FOR EACH SPRITE... (22/02/2020) YOUR WELCOME XO
	call put_pixel ;render out the sprite

	jmp init_update ;call the start of the update loop again

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
	xor ax, ax ;0 ax reg (xor ax, ax = smaller (2b vs 5b) and addition of clearing CF)
	mov  al, [si] ;move curr col byte into al
	;e.g. red 00CC -> AH [0000 0000] AL [*1101* 1101]
	shr al, 04h ;-> AH [0000 0000] AL [0000 *1101*] 
	mov  [es:bx], al

	mov ax, [x] ;x 
	inc ax ;add 1 to x
	mov [x], ax ;write the reg back to the var x 
	mov bx, [orig_x] ;move the target x into the cx reg
	add bx, 16 ;append the 16px (15 = 16px - 1) onto the target to simulate the width of sprite
	cmp ax, bx ;check to see curr x is equal to target x
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

	mov ax, [x]
	inc ax
	mov [x], ax
	mov bx, [orig_x]
	add bx, 16
	cmp ax, bx
	je pixel_new_row

	jmp put_pixel

pixel_new_row: ;used in put pixel to increment y
	mov ax, [orig_x] ;set x to 0
	mov [x], ax ;write the reg back to the var x

	mov ax, [y] ;move y back into ax again, will have been overwritten during pixel placement
	mov bx, [orig_y]
	add bx, 16
	cmp ax, bx ;if its equal to 15 (last row), end
	je init_update ;jump back to the start of the update loop again (device_end returns, in this case it will be from this function)

	inc ax ;plus 1 to the y
	mov [y], ax

	mov ax, [put_pixel_func_flag]
	cmp ax, 01h
	je put_pixel ;if we came out of put_pixel(1), we want to continue to put_pixel2
	jmp put_pixel2 ;else move back to the put_pixel(1) function

device_end:
	ret

waitforRetrace:
    mov dx, 03dah
waitforRetraceEnd:
    in al, dx
    and al, 08h
    jnz waitforRetraceEnd
waitforRetraceStart:
    in al, dx
    and al, 08h
    jz waitforRetraceStart
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