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
    mov ax, [bsSecsPerFat]
    movzx bx, byte [bsFATs]
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
   
    movzx eax, word [rootstart]
    .next_sector:
        mov bx, [destination]
        mov di, bx
        call readsector

        .next_entry:
            ;check filename
            mov cx, 11
            mov si, [filename]
            repe cmpsb
            jz .found      

            ;check if end of sector (di == dir entry +11 now)
            add di, 0x20
            and di, -0x20
            cmp di, [bsBytesPerSec]
            jnz .next_entry
        jnz .next_sector

.found:
    add di, 0xF
    mov ax, [di]   ;ax <- starting cluster

    ;store file size
    add di, 2
    mov edx, [di]
    mov [fileSize], edx

    mov bx, [destination] 
    .next_cluster: 
        call readcluster 
        cmp ax, 0xFFF7 ; eof/badcluster
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
; Read one sector from disk
;   eax -> 32-bit DOS sector number
;   es:bx -> destination
; 
;   es:bx  <- one byte after the last byte read
;   eax  <- next sector
readsector:
    push dx
    push si
    push di

.readSegment:
    push eax   ; Save the sector number

    mov [pbBufferOff], bx
    mov [pbBufferSeg], es
    mov [pbSectorLow], eax

    mov si, paramBlock
    mov dl, [bsDriveNumber]
    mov ah, 42h   ; LBA
    int 0x13 

    pop eax      ; Restore the sector number
    jnc .readed    ; jump if no error

    ;reset disk
    push ax
    xor ah, ah   
    int 0x13
    pop ax

    jmp .readSegment ;try again

.readed:
    inc eax       ; next sector
    add bx, word [bsBytesPerSec]   ; Add bytes per sector
    jnc notOverflow   

    ;overflow
    mov dx, es
    add dh, 0x10      ;add 1000h to ES
    mov es, dx

notOverflow:
    pop di
    pop si
    pop dx
    ret

paramBlock:
   pbSize db  0x10 
   pbNull db  0x00     
   pbNumSectors dw 1  
   pbBufferOff dw  0x0000
   pbBufferSeg dw  0x0000 
   pbSectorLow dd  0x0000
   pbSectorHigh dd 0x0000

;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Read a cluster from disk
;   ax -> cluster number
;   es:bx -> destination
;   
;   es:bx <- byte after the last byte read
;   ax <- next cluster number
readcluster:
    push cx
    mov [currentCluster], ax

    ;calculate starting sector of cluster
    xor cx, cx
    sub ax, 2
    mov cl, [bsSecsPerClust]
    imul cx  
    add ax, word [datastart] 

    mov cl, [bsSecsPerClust]
    .nextsector:
        call readsector
        loop .nextsector

    push bx 

    mov bx, 0x7E00  
    push bx
    mov ax, [bsResSectors]
    call readsector
    pop bx ;7E00  

    mov ax, [currentCluster] ; ax holds the cluster # we just read
    shl ax, 1   
    add bx, ax
    mov ax, [bx]
   
    pop bx  
    pop cx
    ret
;------------------------------------------------------------------------------
currentCluster dw 0x0000

