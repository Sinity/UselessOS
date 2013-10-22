[bits 16]
[org 0x7C00]

jmp start
nop

%define bsOemName   bp+0x03    		; OEM label (8)
%define bsBytesPerSec   bp+0x0B    	; bytes/sector (dw)
%define bsSecsPerClust   bp+0x0D    ; sectors/allocation unit (db)
%define bsResSectors   bp+0x0E    	; # reserved sectors (dw)
%define bsFATs      bp+0x10    		; # of fats (db)
%define bsRootDirEnts   bp+0x11		; Max # of root dir entries (dw)
%define bsSectors   bp+0x13    		; # sectors total in image (dw)
%define bsMedia    bp+0x15    		; media descriptor (db)
%define bsSecsPerFat   bp+0x16  	; # sectors in a fat (dw)
%define bsSecsPerTrack   bp+0x18    ; # sectors/track
%define bsHeads      bp+0x1A    	; # heads (dw)
%define bsHidden    bp+0x1C    		; # hidden sectors (dd)
%define bsSectorHuge   bp+0x20   	; # sectors if > 65536 (dd)
%define bsDriveNumber   bp+0x24    	; (dw)
%define bsSigniture   bp+0x26    	; (db)
%define bsVolumeSerial   bp+0x27    ; (dd)
%define bsVolumeLabel   bp+0x2B    	; (11)
%define bsSysID      bp+0x36    	; (8)

times 0x3B db 0 ; skip bpb

msgGreet	db 'First stage bootloader version 2.67 welcomes you, master.', 13, 10, 0
%include "bootloader/rm_stdio.inc"

read_pocket:
	db	0x10	; size of pocket
	db	0		; const 0
	dw	4		; number of sectors to transfer
	dw	0x1000, 0x0000	; address to write (segment:offset but because its little endian its offset, segment)
	dq	1		; sector to read

start:					
	;----------------------------------------------------
	; SETUP SEGMENT REGISTERS, STACK and direction flag  |
	;----------------------------------------------------
	cli ; Disable interrupts
	
	mov bp, 0x7C00 ; bp is pointer to loaded bootsector

	mov [bsDriveNumber], dl ;save drive number for later.

	; Setup stack
	xor ax, ax
	mov ss, ax
	mov sp, 0x900 ; 0x900 to 0x500. 

	; Setup other segment regs
	mov es, ax
	mov ds, ax
	
	sti ; Enable interrupts
	
	cld ; Clear direction flag

	;----------------------------------------------------
	; 				PRINT WELCOME STRING 				 |
	;----------------------------------------------------
	mov si, msgGreet
	call puts16
	
	;----------------------------------------------------
	; 			LOAD SECOND STAGE TO MEMORY				 |
	;----------------------------------------------------
	mov	si, read_pocket
	mov	ah, 0x42	; function (LBA)
	mov dl, [bsDriveNumber] ; drive number
	int	0x13
	 
	;----------------------------------------------------
	; 				ENTER TO SECOND STAGE!				 |
	;----------------------------------------------------
	mov ax, 0x1000
	jmp ax

;fill rest of bootsector with zeroes
times (510-($-$$)) db 0 ; $ is current line adress, $$ is adress of first line.

;bootsector signature
db 0x55
db 0xAA

;**************************************************
;					Infos                         *
;**************************************************
			
;General memory map in real mode:
;0x00000000 - 0x000003FF - Real Mode Interrupt Vector Table
;0x00000400 - 0x000004FF - BIOS Data Area

;0x00000500 - 0x00007BFF - Unused
;0x00007C00 - 0x00007DFF - Bootsector
;0x00007E00 - 0x0009FFFF - Unused

;0x000A0000 - 0x000BFFFF - Video RAM (VRAM) Memory
;0x000B0000 - 0x000B7777 - Monochrome Video Memory
;0x000B8000 - 0x000BFFFF - Color Video Memory
;0x000C0000 - 0x000C7FFF - Video ROM BIOS
;0x000C8000 - 0x000EFFFF - BIOS Shadow Area
;0x000F0000 - 0x000FFFFF - System BIOS
