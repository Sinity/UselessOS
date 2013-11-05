[bits 32]

%define EhdrMagic 			0  ;dd - 0x7F, 'ELF' 
%define EhdrIdent			4  ;12b
%define EhdrType			16 ;dw
%define EhdrMachine			18 ;dw
%define EhdrVersion			20 ;dd
%define EhdrEntry			24 ;dd
%define EhdrPHptr			28 ;dd
%define EhdrSHptr			32 ;dd
%define	EhdrFlags			36 ;dd			
%define	EhdrSize			40 ;dw
%define	EhdrPHSize			42 ;dw
%define	EhdrPHNum			44 ;dw
%define EhdrSHSize			46 ;dw
%define EhdrSHNum			48 ;dw
%define EhdrSHSTRNDX		50 ;dw

%define PhdrType			0 ;dd
%define PhdrFileOffset		4 ;dd
%define PhdrVAddr			8 ;dd
%define PhdrPAddr			12 ;dd
%define PhdrFileSize		16 ;dd
%define PhdrMemSize			20 ;dd
%define PhdrFlags			24 ;dd
%define PhdrAlign			28 ;dd

%define PH_LOAD 1

%define ShdrName	0 ;dd
%define ShdrType	4 ;dd
%define ShdrFlags	8 ;dd
%define ShdrAddr	12 ;dd
%define ShdrFileOff	16 ;dd
%define ShdrSize	20 ;dd
%define ShdrLink	24 ;dd
%define ShdrInfo	28 ;dd
%define ShdrAlign	32 ;dd
%define ShdrEntSize	36 ;dd

%define SHT_DYNSYM 	11
%define SHT_SYMTAB	2

%define SymtabName	0 ;dd
%define SymtabValue	4 ;dd
%define SymtabSize	8 ;dd
%define SymtabInfo	12 ;b
%define SymtabOther	13 ;b
%define SymtabSHNdx 14 ;dw 


entryPoint: 	dd 0
entryPointAddr: dd 0
;-----------------------------------------
;	Loads elf file 
;Args:
;	ebp - adress of file
;	eax - entrypoint function name ptr
;Ret:
; 	eax - entry point ptr
;-----------------------------------------
LoadElf:
	pushad
    mov [entryPoint], eax

    mov ebx, [ebp + EhdrPHptr]
    add ebx, ebp

    xor ecx, ecx ;current program header index
.phdrLoop:
	;this section should be loaded?
	cmp dword [ebx + PhdrType], PH_LOAD
	jne .nextPH
	cmp dword [ebx + PhdrFileSize], 0
	je .nextPH

	;yes, load it
	mov esi, [ebx + PhdrFileOffset]
	add esi, ebp ; esi == file ptr + section offset == section to copy
	mov edi, [ebx + PhdrVAddr] ; edi == section destination
	push ecx
	mov ecx, [ebx + PhdrFileSize]
	rep movsb
	pop ecx

	;need fill memory with zeroes?
	mov eax, [ebx + PhdrMemSize]
	sub eax, [ebx + PhdrFileSize]
	jng .nextPH

	;yes, fill it
.fillZeroes:
	mov byte [edi], 0
	inc edi
	dec eax
	cmp eax, 0
	je .fillZeroes

.nextPH:
	;calculate adress of next program header
	movzx eax, word [ebp + EhdrPHSize]
	add ebx, eax
	
	inc cx
	cmp cx, word [ebp + EhdrPHNum]
	jne .phdrLoop

	;-------
	; Find entry point
	;-------

	mov ebx, [ebp + EhdrSHptr]
	add ebx, ebp ; ebx == shdr* 
	xor ecx, ecx
.ShdrLoop:
	cmp dword [ebx + ShdrType], SHT_DYNSYM
	je .segFound
	cmp dword [ebx + ShdrType], SHT_SYMTAB
	jne .nextShdr

.segFound:
	;edx -> string table
	mov eax, [ebx + ShdrLink]
	movzx edx, word [ebp + EhdrSHSize]
	mul edx ; eax -> offset from begenning of section headers table
	mov edx, [ebp + EhdrSHptr]
	add edx, eax
	add edx, ebp
	mov edx, [edx + ShdrFileOff]
	add edx, ebp

	;eax -> symbol table
	mov eax, [ebx + ShdrFileOff]
	add eax, ebp

	push ecx
	xor ecx, ecx
.nextEntry:
	mov esi, [entryPoint]
	mov edi, edx
	add edi, [eax + SymtabName]
	push eax
	call strcmp
	mov esi, eax ;esi <= result of comparision
	pop eax

	cmp esi, 0
	je .entryFound

	add ecx, [ebx + ShdrEntSize]
	add eax, [ebx + ShdrEntSize]
	cmp ecx, [ebx + ShdrSize]
	jb .nextEntry

.entryFound:
	pop ecx
	mov eax, [eax + SymtabValue] 
	mov [entryPointAddr], eax
	jmp .end

.nextShdr:
	;ebx == next section header
	movzx eax, word [ebp + EhdrSHSize]
	add ebx, eax

	;more section headers exist?
	inc cx
	cmp cx, [ebp + EhdrSHNum]
	jne .ShdrLoop

.end:
	popad
	mov eax, [entryPointAddr]
	ret

;------------------------------------------------------------------------
;esi -> adress of first null-terminated string
;edi -> adress of second null-terminated string
;eax <- 0 if strings are equal, 1 otherwise
;esi, edi <- first mismached char if not equal, otherwise 0-terminator
strcmp:

.nextChar:
	mov ah, [esi]
	cmp ah, [edi]
	jne .notEqual
	cmp ah, 0
	jz .equal
	inc esi
	inc edi
	jmp .nextChar

.equal:
	mov eax, 0
	ret

.notEqual:
	mov eax, 1
	ret