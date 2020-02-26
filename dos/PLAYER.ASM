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

jump_player: 
	cmp ax, 03920h ;test if the spacebar is pressed
	je inc_jump_player ;play out the jump routine

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

inc_jump_player:
	mov al, [player_jump_flag]
	inc al
	mov [player_jump_flag], al
	ret

player_jump:
	int 021h
	ret

section .data ;put data here
	
section .bss ;put uninit data here