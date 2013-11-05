[bits 16]

%define bsOemName   bp+0x03         ; OEM label (dq)
%define bsBytesPerSec   bp+0x0B     ; bytes/sector (dw)
%define bsSecsPerClust   bp+0x0D    ; sectors/allocation unit (db)
%define bsResSectors   bp+0x0E      ; # reserved sectors (dw)
%define bsFATs      bp+0x10         ; # of FATs (db)
%define bsRootDirEnts   bp+0x11     ; Max # of root dir entries (dw)
%define bsSectors   bp+0x13         ; # sectors total in image (dw)
%define bsMedia    bp+0x15          ; media descriptor (db)
%define bsSecsPerFat   bp+0x16      ; # sectors in a fat (dw)
%define bsSecsPerTrack   bp+0x18    ; # sectors/track
%define bsHeads      bp+0x1A        ; # heads (dw)
%define bsHidden    bp+0x1C         ; # hidden sectors (dd)
%define bsSectorHuge   bp+0x20      ; # sectors if > 65536 (dd)
%define bsDriveNumber   bp+0x24     ; drive number, 0x80 of 0x81 for HDD(dw)
%define bsSigniture   bp+0x26       ; reserved (db)
%define bsVolumeSerial   bp+0x27    ; used for tracking volume between computers(dd)
%define bsVolumeLabel   bp+0x2B     ; disc label, padded with spaces(11)
%define bsSysID      bp+0x36        ; filesystem id(8)

;******************************************
;si -> filename ptr
;di -> destination ptr
;bp -> bootsector ptr

;eax <- size of file in bytes
;******************************************
loadFile:
  pusha
  mov [filename], si
	mov [destination], di
  
	;rootstart = bsResSectors + (bsFATs * bsSecsPerFat)
  movzx eax, word [bsSecsPerFat]
	movzx ebx, byte [bsFATs]
	mul bx
  add ax, [bsResSectors]
  mov [rootstart], ax

	;datastart = rootstart + ((bsRootDirEnts * 32) / bsBytesPerSec) 
  mov ax, [bsRootDirEnts]
  shl ax, 5
	mov bx, [bsBytesPerSec]
	mov dx, 0
	div bx
  add ax, [rootstart]   
  mov [datastart], ax
   

  xor edx, edx
  movzx eax, word [rootstart]

.next_sector:
  movzx eax, word [rootstart]
  add eax, edx
  mov bx, [destination]
  mov si, bx
  mov di, bx
  call readsector

  ;load entry from root table
  .next_entry:
	  ;check filename
    mov cx, 11
    mov si, [filename]
    repe cmpsb
    jz .found      

	  ;di == dir entry +11

	  ;check if end of sector
    add di, 0x20
    and di, -0x20
    cmp di, [bsBytesPerSec]
    jnz .next_entry

	  ;read next sector
    inc edx
    jnz .next_sector

.found:
  add di, 15
  mov ax, [di]   ;ax now holds the starting cluster

  ;store file size
  movzx edx, di
  add edx, 2
  mov edx, [edx]
  mov [fileSize], edx

  mov bx, [destination] 
  xor ecx, ecx

  .next_cluster: 
	  call readcluster
	  inc ecx
    cmp ax, 0xFFF7 ; Have we reached the eof or bad cluster?
    jb .next_cluster 

	popa
	mov eax, [fileSize]
	ret

fileSize 	dd 0x0000
destination dw 0x0000
filename 	dw 0x0000
datastart	dw 0x0000
rootstart 	dw 0x0000

;------------------------------------------------------------------------------
; Read a sector from disk, using LBA
; input:   EAX - 32-bit DOS sector number
;      ES:BX - destination buffer
; output:   ES:BX points one byte after the last byte read
;      EAX - next sector
readsector:
   push dx
   push si
   push di

read_it:
   push eax   ; Save the sector number
   mov di, sp   ; remember parameter block end

   push byte 0   ; other half of the 32 bits at [C]
   push byte 0   ; [C] sector number high 32bit
   push eax   ; [8] sector number low 32bit
   push es    ; [6] buffer segment
   push bx    ; [4] buffer offset
   push byte 1   ; [2] 1 sector (word)
   push byte 16   ; [0] size of parameter block (word)

   mov si, sp
   mov dl, [bsDriveNumber]
   mov ah, 42h   ; LBA
   int 0x13 

   mov sp, di   ; remove parameter block from stack
   pop eax      ; Restore the sector number

   jnc read_ok    ; jump if no error

   push ax
   xor ah, ah   ; else, reset and retry
   int 0x13
   pop ax
   jmp read_it

read_ok:
   inc eax       ; next sector
   add bx, 512      ; Add bytes per sector
   jnc no_incr_es      ; if overflow...

incr_es:
   mov dx, es
   add dh, 0x10      ; ...add 1000h to ES
   mov es, dx

no_incr_es:
   pop di
   pop si
   pop dx
   ret

;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Read a cluster from disk, using LBA
; input:   AX - 16-bit cluster number
;      ES:BX - destination buffer
; output:   ES:BX points one byte after the last byte read
;      AX - next cluster
readcluster:
   push cx
   mov [tmpcluster], ax

   ;calculate starting sector of cluster
   xor cx, cx
   sub ax, 2
   mov cl, [bsSecsPerClust]
   imul cx  
   add ax, word [datastart] 

   xor cx, cx
   mov cl, byte [bsSecsPerClust]

  .nextsector:
   call readsector
   dec cx
   cmp cx, 0
   jne .nextsector

   push bx 
   mov bx, 0x7E00  
   push bx
   mov ax, [bsResSectors]
   call readsector
   pop bx  
   mov ax, [tmpcluster] ; ax holds the cluster # we just read
   shl ax, 1   
   add bx, ax
   mov ax, [bx]
   
   pop bx  
   pop cx
   
   ret
;------------------------------------------------------------------------------
tmpcluster dw 0x0000

