section .text

;1F73 'w' / 'W'(unsure atm??)
;1177 's' / 'S' 
;1E61 'a' / 'A'
;2064 'd' / 'D'

get_key:
	xor ax, ax ;0 ax reg
	mov ah, 000h ;check if key pressed without stopping program
	int 16h ;keyboard read interrupt
	ret

move_player_d: ;this is called which does the 'd' key check first, then runs downwards through key funcs
	cmp ax, 02064h ;test if 'd' is pressed
	je inc_x_player ;if 'd' is pressed, x += 1

move_player_a:
	cmp ax, 01E61h ;test if 'a' is pressed
	je dec_x_player ;if 'a' is  pressed, x -= 1

move_player_w:
	cmp ax, 01F73h ;test if 'w' is pressed
	je inc_y_player ;if 'w' is pressed, y += 1

move_player_s:
	cmp ax, 01177h ;test if 's' is pressed
	je dec_y_player ;if 's' is  pressed, y -= 1

move_player_ret: ;must call the key press condition is not met (for program flow)
	ret ;return out of the move_player_[X] func as we call it 

;orig_[X] and [X] must be kept synced, always change both
inc_x_player:
	mov ax, [x]
	add ax, 05h ; x += 1
	mov [x], ax
	mov ax, [orig_x]
	inc ax ; orig_x += 1
	mov [orig_x], ax
	ret

dec_x_player:
	mov ax, [x]
	sub ax, 05h ; x -= 1
	mov [x], ax
	mov ax, [orig_x]
	dec ax ; orig_x -= 1
	mov [orig_x], ax
	ret

inc_y_player:
	mov ax, [y]
	add ax, 05h ; y += 1
	mov [y], ax
	mov ax, [orig_y]
	inc ax ; y += 1
	mov [orig_y], ax
	ret

dec_y_player:
	mov ax, [y]
	sub ax, 05h ; y -= 1
	mov [y], ax
	mov ax, [orig_y]
	dec ax ; orig_y -= 1
	mov [orig_y], ax
	ret

section .data ;put data here
	
section .bss ;put uninit data here