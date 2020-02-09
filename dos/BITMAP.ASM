
;http://forums.codeguru.com/showthread.php?444694-Read-file-from-assembly
;http://jlp.freeservers.com/files/index.htm
;http://devdocs.inightmare.org/tutorials/x86-assembly-dos-file-inputoutput.html

section .text

load_bmp_open_file:
	mov ax, 03d00h ;open file / read only
	mov dx, filename ;offset to file
	int 021h

	;This is dumb... mov bx, ax doesnt work...
	mov [handle], ax
	
	mov ah, 03fh	;read file function
	mov bx, [handle]
	mov dx, data_ptr
	mov cx, 54	; read 54 bytes
	int 021h	; read file, AX = bytes read
	
	cmp ax, 0 ;AL(or AX) shows hows many bytes were read...
	je device_end

	mov ah, 03eh ;close file
	mov bx, handle
	int 021h

	jmp write_bmp_head

write_bmp_head:
	mov ax, 03c01h
	mov cx, 0
	mov dx, filename_write
	int 021h

	mov bx, ax
	mov ah, 040h
	mov dx, data_ptr
	mov cx, 54
	int 021h

	mov ah, 03eh ;close file
	mov bx, handle
	int 021h

	jmp vga_main

section .data ;put data here
	handle dw 0
	data_ptr times 64 db 0
	filename db 'TEST.BMP', 00h
	filename_write db 'DUMP.TXT', 00h
section .bss ;put uninit data here