;https://thestarman.pcministry.com/asm/debug/8086REGs.htm#REGS
;https://everything2.com/title/VGA+Mode+13h

main:
	org	100h ;code starts at offset 100h
	use16 ;use 16-bit code

	mov ah, 00h ;VGA mode
	mov al, 013h 
	int 10h ; 320x200x8bit

	mov ax, 0A000h ;video memory address SVGA
	mov es, ax ;ES register now points to video memory
	mov si, pixels
	mov ax, [si] ;y
	mov bx, [si+02h] ;x

	jmp put_pixel

put_pixel:
	mov cx, 320 ;scr width
	mul cx ;mul cx (cx *= ax (dest reg))
	add ax, bx
	mov di, ax             
	mov dl, 4           
	mov [es:di], dx ;write pixel to es dest vga buffer

	add si, 04h ;skip over 4 bytes (2x 2 bytes), next set of x and y
	mov ax, [si] ;y
	mov bx, [si+02h] ;x

	cmp ax, 53 ;if its equal to 53 (last elem, cancel), else re run
	je device_end

	jmp put_pixel

device_end:
	int 10h
	ret

pixels dw 50, 50, 51, 51, 52, 52, 53, 53