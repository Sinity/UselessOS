[bits 16] ; still in real mode.
[org 0x1000] ; first stage bootloader loads this to 0x0:0x1000

%define KERNEL_RMODE_BASE_PTR 0x3000 	; where kernel image will be temporally loaded in real mode

jmp start

msgWelcome		db "I'm second stage bootloader ver. 2.08, we're going to 32 bit world!", 0x0A, 0x00

%include "bootloader/rm_stdio.inc" 
%include "bootloader/gdt.inc"	
%include "bootloader/A20.inc"
%include "bootloader/FAT16.asm"

kernelName db "KRNL    SYS" 	; kernel name (Must be 11 bytes)

start:
	mov si, msgWelcome
	call puts16

	cli	;disable interrupts - in protected mode we cannot access to IVT and any interrupt will give us triple fault
 
	call	installGDT		
	call	enableA20	

	;load kernel image
	mov bp, 0x7c00
	mov si, kernelName
	mov di, KERNEL_RMODE_BASE_PTR
	call loadFile

	enterPMODE:
		mov	eax, cr0		
		or	eax, 1
		mov	cr0, eax	 
		jmp	0x8:stage3		; far jump to fix CS. 0x8 is offset in GDT, not offset like in real mode.

;********************************************************************************************************************
; 											STAGE 3 ENTRY POINT!													|
;********************************************************************************************************************

[bits 32]
msgEnter32bit db "After a long travel, we're in the protected mode. Yay!", 0x0A, 0x00
%include "bootloader/stdio.inc" 
%include "bootloader/elf.asm"
entryPointFunc db 'kmain', 0

stage3:
	; fix rest of segment registers - set data segments to data selector
	mov		eax, 0x10
	mov		ds, ax
	mov		ss, ax
	mov		es, ax

	mov		esp, 0x90000		; stack begins from 0x90000. Grows downwards.
 

	call cls
	mov esi, msgEnter32bit
	call puts32

   	;load kernel
   	mov ebp, KERNEL_RMODE_BASE_PTR
   	mov eax, entryPointFunc
   	call LoadElf
   	
   	call cls
    jmp eax