section .text

;1F73 'w' / 'W'(unsure atm??)
;1177 's' / 'S' 
;1E61 'a' / 'A'
;2064 'd' / 'D'
;3920 'SPACE'

get_key:
	xor ax, ax ;0 ax reg
	mov ah, 000h ;check if key pressed without stopping program
	int 16h ;keyboard read interrupt
	ret

move_player_d: ;this is called which does the 'd' key check first, then runs downwards through key funcs
	cmp ax, 02064h ;test if 'd' is pressed
	je inc_x_player ;if 'd' is pressed, x += 1
	ret

move_player_a:
	cmp ax, 01E61h ;test if 'a' is pressed
	je dec_x_player ;if 'a' is  pressed, x -= 1
	ret

move_player_w:
	cmp ax, 01177h ;test if 'w' is pressed
	je inc_y_player ;if 'w' is pressed, y += 1
	ret

move_player_s:
	cmp ax, 01F73h ;test if 's' is pressed
	je dec_y_player ;if 's' is  pressed, y -= 1
	ret

player_jump_key_press: 
	cmp ax, 03920h ;test if the spacebar is pressed
	je toggle_player_jump_flag ;play out the jump routine
	ret

move_player_ret: ;must call the key press condition is not met (for program flow)
	ret ;return out of the move_player_[X] func as we call it 

;orig_[X] and [X] must be kept synced, always change both
inc_x_player:
	mov ax, [di+00h]
	add ax, 01h ; x += 1
	mov [di+00h], ax
	mov ax, [di+04h]
	add ax, 01h ; orig_x += 1
	mov [di+04h], ax
	ret

dec_x_player:
	mov ax, [di+00h]
	sub ax, 01h ; x -= 1
	mov [di+00h], ax
	mov ax, [di+04h]
	sub ax, 01h ; orig_x -= 1
	mov [di+04h], ax
	ret

inc_y_player:
	mov ax, [di+02h]
	sub ax, 01h ; y += 1
	mov [di+02h], ax
	mov ax, [di+06h]
	sub ax, 01h ; y += 1
	mov [di+06h], ax
	ret

dec_y_player:
	mov ax, [di+02h]
	add ax, 01h ; y -= 1
	mov [di+02h], ax
	mov ax, [di+06h]
	add ax, 01h ; orig_y -= 1
	mov [di+06h], ax
	ret

toggle_player_jump_flag: ;when the player presses the space we need to toggle it on (inc val 0 to 1)
	mov al, [player_jump_flag]
	mov al, 01h
	mov [player_jump_flag], al
	ret

player_jump:
	mov al, [player_jump_flag]
	cmp al, 01h
	je player_jump_isground
	ret

;check if our players x is the same as the ground value,
;if so, set the player's is ground flag to true, else if not we are mid jump
player_jump_isground:
	mov bx, [player_is_grounded] ;check to see if we've jumped
	mov ax, [di+02h] ;move the players y into AL (conv [di] from word to byte) XX 0X
	cmp bx, 00h ;if we've already jumped, go to jump sequence
	je player_jump_from_ground
	dec bx ; player_is_grounded = 0 (we are jumping)
	mov [player_is_grounded], bx ;if we havent jumped, make sure we check the flag that we have jumped
	mov [player_orig_y_pos], ax ;log the players y position to revert back
	je player_jump_from_ground
	ret

player_jump_from_ground:
	mov bx, [player_max_y]
	cmp ax, bx
	jle player_jump_fall ;if the player has met or surpassed the max height
	mov bx, [player_is_falling]
	cmp bx, 00h
	je player_jump_up
	jmp player_jump_fall
	ret

player_jump_up:
	sub ax, 01h ;increment the player jump value 
	mov [di+02h], ax ;move the new y to the y
	mov [di+06h], ax ;move the new y to the orig_y
	ret

player_jump_fall:
	;make sure we update it, so we dont fall 1 pixel and then climb again
	cmp bx, 00h
	je player_jump_is_falling_flag_on
	add ax, 01h ;decrement the player down as we fall
	mov [di+02h], ax ;move the new y to the y
	mov [di+06h], ax ;move the new y to the orig_y
	mov bx, [player_orig_y_pos] 
	cmp ax, bx
	jge player_jump_done
	ret

player_jump_is_falling_flag_on:
	mov bx, 01h
	mov [player_is_falling], bx
	ret

player_jump_done:
	mov ax, [player_jump_flag] ;reset the jumping flag
	mov ax, 00h
	mov [player_jump_flag], ax
	mov ax, [player_is_falling] ;reset the falling flag
	mov ax, 00h
	mov [player_is_falling], ax
	mov ax, [player_is_grounded] ;reset the grounded flag
	mov ax, 01h
	mov [player_is_grounded], ax
	ret

section .data ;put data here
	player_orig_y_pos dw 0 ;log the players y before he jumps
	player_max_y dw 5 ;the maximum height the player can jump too from start pos
	player_is_grounded db 1 ;the flag that tells us if the player is grounded
	player_is_falling db 0 ;the flag when the top has been reached and we're falling

section .bss ;put uninit data here