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

move_player_ret: ;must call the key press condition is not met (for program flow)
	ret ;return out of the move_player_[X] func as we call it 

;resets the speed vectors each frame so we dont inf move
move_player_reset_speed:
	mov ax, [speed_x]
	xor ax, ax
	mov [speed_x], ax
	mov ax, [speed_y]
	xor ax, ax
	mov [speed_y], ax
	ret

;this function is used to sync the speed vectors to the position
move_player_update:
	mov ax, [di+00h] ;x
	mov bx, [speed_x]
	add ax, bx
	mov [di+00h], ax ;move new speed to x
	mov [di+04h], ax ;move new speed to orig_x
	mov ax, [di+02h] ;y
	mov bx, [speed_y]
	add ax, bx
	mov [di+02h], ax ;move new speed to y
	mov [di+06h], ax ;move new speed to orig_y
	ret

;orig_[X] and [X] must be kept synced, always change both
inc_x_player:
	mov ax, [speed_x]
	add ax, 01h ; x += 1
	mov [speed_x], ax
	ret

dec_x_player:
	mov ax, [speed_x]
	sub ax, 01h ; x -= 1
	mov [speed_x], ax
	ret

section .data ;put data here
	speed_x dw 0
	speed_y dw 0
	gravity dw 1 ;how fast we call
	gravity_speed dw 0 ;append gravity to this value

section .bss ;put uninit data here