%ifndef STDIO
%define STDIO

[bits 16]
;---------------------------------------------------------------
; PUTS16 - write string on the screen using int 0x10 interrupt  |
; 	arg: string termined with zero, in ds:si.					|
;   USED RESOURCES: ax register.								|
;---------------------------------------------------------------
puts16:	
	lodsb	; load single byte from ds:si to al
	or al, al ; zero=end or string(terminator)
	jz done
	mov ah, 0x0E	
	int 0x10		; write next letter
	jmp puts16	; write next letter
	done:
		ret
	
%endif ;STDIO
