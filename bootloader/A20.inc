[bits 16]

;--------------------------------------------
; Enables a20 line through output port
;--------------------------------------------

enableA20:
        call    waitInput
        mov     al, 0xAD
        out     0x64, al	; disable keyboard
        call    waitInput

        mov     al, 0xD0
        out     0x64, al	; tell controller to read output port
        call    waitOutput

        in      al,0x60
        push    eax		; get output port data and store it
        call    waitInput

        mov     al,0xD1
        out     0x64,al		; tell controller to write output port
        call    waitInput

        pop     eax
        or      al, 2		; set bit 1 (enable a20)
        out     0x60,al		; write out data back to the output port

        call    waitInput
        mov     al,0xAE		; enable keyboard
        out     0x64,al

        call    waitInput
        ret

; wait for input buffer to be clear
waitInput:
        in      al,0x64
        test    al,2		
        jnz     waitInput ; input full
        ret

; wait for results in output buffer
waitOutput:
        in      al, 0x64
        test    al, 1
        jz      waitOutput ;output empty
        ret
