[bits 32]

%define VIDMEM_PTR 0xB8000
%define COLS 80
%define ROWS 25
%define WHITE_ON_BLACK_ATTRIB 15

;current position on screen
cursorX db 0
cursorY db 0

attribute db WHITE_ON_BLACK_ATTRIB

;==========================================.
;		puts32 - put string on screen	   |
;	esi<--pointer to 0-terminated string   |
;==========================================. 
puts32:
	pusha
	xor bx, bx
.loop:
	mov bl, [esi] ; bl == current letter
	cmp bl, 0
	je .done
	call putc32
	inc esi
	jmp .loop
.done:
	;update cursor position
	mov bl, [cursorX]
	mov bh, [cursorY]
	call moveCursor

	popa
	ret

;========================
;  cls - clears screen	|
;========================
cls:
	mov ecx, COLS*ROWS*2 ; 2 bytes per character
	mov bl, [attribute]
.loopcls:
	mov [VIDMEM_PTR + ecx], byte 0
	dec ecx
	mov [VIDMEM_PTR + ecx], bl
	loop .loopcls
	mov [VIDMEM_PTR], byte 0

	mov bx, 0
	call moveCursor

	ret

;====================================
;	changeAttribute					|
;		param: al - new attribute	|
;====================================
changeAttribute:
	mov [attribute], al
	ret


;===============================================================
; moveCursor - moves blinking cursor to the desired position   |
;	param bh - y position									   |
;	param bl - x position									   |
;===============================================================
%define CRT_INDEX  0x03D4
%define CRT_DATA 0x03D5
moveCursor: 
	pusha
	xor edx, edx 

	mov [cursorY], bh 
	mov [cursorX], bl

	;calculate linear offset
	mov eax, COLS
	mov dl, bh
	mul	edx
	xor dl, bl			
	add	eax, edx
	mov	ebx, eax
 
	;low byte
	mov	al, 0x0f		
	mov	dx, CRT_INDEX	
	out	dx, al
 
	mov	al, bl			
	mov	dx, CRT_DATA	
	out	dx, al		
 
	xor	eax, eax
 
	;high byte
	mov	al, 0x0E		
	mov	dx, CRT_INDEX
	out	dx, al
 
	mov	al, bh		
	mov	dx, CRT_DATA		
	out	dx, al		

	popa
	ret

;============================================
;	putc32 - put char on screen				 |
;	param bl - character					 |				
;============================================
putc32:
	pusha

	mov edi, VIDMEM_PTR
	
	xor ecx, ecx
	xor edx, edx

	; calculate vertical position
	mov eax, COLS*2
	mov dl, [cursorY]
	mul edx
	mov esi, eax

	; calculate horizontal position
	xor eax, eax
	mov al, [cursorX]
	mov ecx, 2
	mul ecx
	
	; calculate linear offset and apply it to EDI
	add eax, esi
	add edi, eax

	;check if character is newline
	cmp bl, 0xA
	je .row

	; 'print' character
	mov bh, [attribute]
	mov [edi], bx

	inc byte [cursorX] ;move cursor to next position

	; check for horizontal overflow.
	cmp byte [cursorX], COLS
	jna .done

.row:
	inc byte [cursorY]
	mov [cursorX], byte 0
	
.done:
	popa
	ret


; :D 
purplescreen:
	pusha
	
	;cls with purple color
	mov al, 5
	shl al, 4
	or al, 14
	call changeAttribute
	call cls

	jmp bluDone
	purpleskrin db 'this. is. PURPLESCREEN!!!', 0
	bluDone:

	;set cursor nearly center
	mov bh, ROWS / 2 - 2
	mov bl, COLS / 3 - 2
	call moveCursor

	;write text
	mov esi, purpleskrin
	call puts32

	popa
	ret
