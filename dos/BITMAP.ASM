include 'include/macro/struct.inc'

struct BMPHEAD ;BMP head structure
	id db ?
	filesize dw ?
	reserved dw ?
	headersize dw ?
	infosize dw ?
	width dw ?
	depth dw ?
	biplanes dw ?
	bits dw ?
	bicomp dw ?
	bisizeim dw ?
	bixpels dw ?
	biclrused dw ?
	biclrimp dw ?
ends

struct RGBQUAD ;how the bitmap stores its colors (BGR)
	b db ?
	g db ?
	r db ?
	fill db ?
ends

load_bmp_open_file:
	mov ax, 03d00h ;open file / read only
	lea dx, [filename] ;offset to file
	int 021h
	mov [handle], ax
	jnc load_bmp_head
	ret

load_bmp_head:	
	mov ah, 03fh	;read file function
	mov bx, [handle] ; get handle
	mov cx, 54	; read 54 bytes
	lea dx, [data_ptr]
	int 021h	; read file, AX = bytes read
	
	cmp ax, 0 ;AL(or AX) shows hows many bytes were read...
	je device_end

	mov ah, 03eh ;close file
	mov bx, handle
	int 021h

	jmp vga_main

filename db 'TEST.BMP', 024h ;0 terminating string
handle dw 0
data_ptr db 54 dup (0)

;http://forums.codeguru.com/showthread.php?444694-Read-file-from-assembly
;http://jlp.freeservers.com/files/index.htm
;http://devdocs.inightmare.org/tutorials/x86-assembly-dos-file-inputoutput.html