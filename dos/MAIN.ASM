;https://thestarman.pcministry.com/asm/debug/8086REGs.htm#REGS
;https://everything2.com/title/VGA+Mode+13h

section .text
org 100h
global _start
bits 16
_start: ;put code here
	mov dx, player_fn ;file name to load
	call load_bmp_open_file ;load the file and pointer
	mov dx, player_spr ;the sprite we want to load the data into
	call load_bmp_data ;call the bitmap data loader from the file we've opened
	
	mov dx, platform_fn ;load the platform
	call load_bmp_open_file
	mov dx, platform_spr
	call load_bmp_data
	
	jmp vga_main

;8088 4.77 MHz 	315 cycles on DOSBOX
vga_main:
	;used as a clear screen
	mov ax, 0013h ;VGA mode
	int 10h ; 320x200x256col

	mov ax, 0A000h ;video memory address SVGA
	mov es, ax ;ES register now points to video memory

	mov si, platform_spr
	mov di, pos_array

	jmp init_update

init_update: ;call this before calling update loop, resets all vars
	;call waitforRetrace  ; destroys al, dx

	mov bx, [di+04h] ;reset x to orig_x
	mov [di+00h], bx
	mov bx, [di+06h] ;reset y to orig_y
	mov [di+02h], bx

	jmp update ;jump to update

update: ;loop through all update/render funcs
	call get_key ;AX will hold the key we pressed...
	
	call move_player_d ;MUST CALL STRAIGHT AFTER 'get_key', move the player based on the ax value
	call move_player_a
	;call move_player_w
	;call move_player_s

	;check to see if the jump flag has been set from the spacebar
	;then call the jump routine which will check and perform a jump
	call player_jump_key_press
	call player_jump

	mov si, platform_spr
	mov di, pos_array+08h
	call put_pixel

	;render the player last because the user input works for the last sprite drawn
	mov si, player_spr ;reset the si pointer to start of the bmp pixel data
	mov di, pos_array ;player sprite is base 0 offset in pos array
	call put_pixel ;render out the sprite

	jmp init_update ;call the start of the update loop again

put_pixel:
	;post pixel_new_row func, we need to know where to continue from pixel data wise
	;put_pixel (1) sets the flag to 0
	xor ax, ax  ;0 ax reg (xor ax, ax = smaller (2b vs 5b) and addition of clearing CF)
	mov [put_pixel_func_flag], ax 

	mov  ax, 320
	imul  ax, [di+02h] ;320*y
	add ax, [di+00h] ;x
	inc ax ; x + 1
	mov  bx, ax ;320*y+x into bx reg
	xor ax, ax ;0 ax reg (xor ax, ax = smaller (2b vs 5b) and addition of clearing CF)
	mov  al, [si] ;move curr col byte into al
	;e.g. red 00CC -> AH [0000 0000] AL [*1101* 1101]
	shr al, 04h ;-> AH [0000 0000] AL [0000 *1101*] 
	mov  [es:bx], al

	mov ax, [di+00h] ;x 
	inc ax ;add 1 to x
	mov [di+00h], ax ;write the reg back to the var x 
	mov bx, [di+04h] ;move the target x into the cx reg
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
	imul ax, [di+02h]
	add ax, [di+00h]
	inc ax ; x + 1
	mov bx, ax
	xor ax, ax ;0 ax reg (xor ax, ax = smaller (2b vs 5b) and addition of clearing CF)
	mov  al, [si] ;move curr col byte into al
	;e.g. red 00CC -> AH [0000 0000] AL [1101 *1101*]
	shl al, 04h ;-> AH [0000 0000] AL [*1101* 0000] 
	shr al, 04h ;-> AH [0000 0000] AL [0000 *1101*] 
	mov  [es:bx], al ;

	add si, 01h ;add 1 byte increment (8 bit shift, 2x 4 bit colours, hence we do 2x pixels in put_pixel)

	mov ax, [di+00h]
	inc ax
	mov [di+00h], ax
	mov bx, [di+04h]
	add bx, 16
	cmp ax, bx
	je pixel_new_row

	jmp put_pixel

pixel_new_row: ;used in put pixel to increment y
	mov ax, [di+04h] ;set x to 0
	mov [di+00h], ax ;write the reg back to the var x

	mov ax, [di+02h] ;move y back into ax again, will have been overwritten during pixel placement
	mov bx, [di+06h]
	add bx, 16
	cmp ax, bx ;if its equal to 15 (last row), end
	je put_pixel_end ;jump back to the start of the update loop again (device_end returns, in this case it will be from this function)

	inc ax ;plus 1 to the y
	mov [di+02h], ax

	mov ax, [put_pixel_func_flag]
	cmp ax, 01h
	je put_pixel ;if we came out of put_pixel(1), we want to continue to put_pixel2
	jmp put_pixel2 ;else move back to the put_pixel(1) function	

put_pixel_end: ;return from the function (under a diff name)
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
	;x, y, orig_x, orig_y format
	;for x & y (& orig_) set -1 of dest coords, we have to increment x off the bat in put_pixel for 2x checks
	pos_array dw 20, 20, 20, 20, 20, 37, 20, 36
	pos_array_index db 0 ;used to offset the pos_array above (+=8 -=8 (4 words = 8 bytes))

	player_jump_flag db 0 ;this is a jump boolean for the player jumping up
	put_pixel_func_flag db 0 ;don't modify! this track which put_pixel we return to after pixel_new_row

section .bss ;put uninit data here

%include 'BITMAP.ASM'
%include 'PLAYER.ASM'