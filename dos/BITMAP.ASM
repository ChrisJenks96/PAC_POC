
;http://forums.codeguru.com/showthread.php?444694-Read-file-from-assembly
;http://jlp.freeservers.com/files/index.htm
;http://devdocs.inightmare.org/tutorials/x86-assembly-dos-file-inputoutput.html

section .text

load_bmp_open_file:
	mov ax, 03d00h ;open file / read only
	int 021h

	mov [handle], ax
	mov ah, 03fh	;read bmp pixels
	mov bx, [handle]
	ret

load_bmp_data:
	mov cx, 128
	int 021h
	cmp ax, 0
	je device_end
	mov ah, 03eh ;close file
	mov bx, handle
	int 021h
	ret

section .data ;put data here
	;GOT 640 KB OF SPACE AVAILABLE (WHEN COMPILED UNDER .COM)
	
	player_fn db 'PLAYER.BIN', 00h
	player_spr times 128 dw 0 ;(16 x 16 mario test)

	platform_fn db 'PLATFORM.BIN', 00h
	platform_spr times 128 dw 0

	handle dw 0
section .bss ;put uninit data here