section .text

;1177 'w' / 'W'(unsure atm??)
;1F73 's' / 'S' 
;1E61 'a' / 'A'
;2064 'd' / 'D'

move_player: ;this is called which does the 'd' key check first, then runs downwards through key funcs
	cmp ax, 02064h ;test if 'd' is pressed
	jnz move_player_a ;if 'd' is not pressed, move to next button test
	call inc_x_player ;if 'd' is pressed
	jmp move_player_ret

move_player_a:
	cmp ax, 01E61h ;test if 'a' is pressed
	jnz move_player_w ;if 'a' is not pressed, move to next button test
	call dec_x_player ;if 'a' is pressed
	jmp move_player_ret ;quit the function call anyways

move_player_w:
	cmp ax, 01177h ;test if 'w' is pressed
	;jnz move_player_s ;if 'w' is not pressed, move to next button test
	;call inc_y_player ;if 'w' is pressed
	;jmp move_player_ret ;quit the function call anyways

	jnz inc_y_player

move_player_s:
	cmp ax, 01F73h ;test if 's' is pressed
	jnz move_player_ret ;if 's' is not pressed, move to next button test
	call dec_y_player ;if 's' is pressed
	jmp move_player_ret ;quit the function call anyways

move_player_ret: ;must call the key press condition is not met (for program flow)
	ret ;return out of the move_player_[X] func as we call it 

;orig_[X] and [X] must be kept synced, always change both
inc_x_player:
	mov ax, [x]
	inc ax ; x += 1
	mov [x], ax
	mov ax, [orig_x]
	inc ax ; orig_x += 1
	mov [orig_x], ax

dec_x_player:
	mov ax, [x]
	dec ax ; x -= 1
	mov [x], ax
	mov ax, [orig_x]
	dec ax ; orig_x -= 1
	mov [orig_x], ax

inc_y_player:
	mov ax, [y]
	inc ax ; y += 1
	mov [y], ax
	mov ax, [orig_y]
	inc ax ; y += 1
	mov [orig_y], ax
	ret ;TMP REMOVE ME

dec_y_player:
	mov ax, [y]
	dec ax ; y -= 1
	mov [y], ax
	mov ax, [orig_y]
	dec ax ; orig_y -= 1
	mov [orig_y], ax

get_key:
	xor ax, ax ;0 ax reg
	mov ah, 00bh ;check if key pressed without stopping program
	int 16h ;keyboard read interrupt
	ret

section .data ;put data here
	
section .bss ;put uninit data here