[bits 32]

extern idtr

global loadIDT
loadIDT:
    lidt [idtr]
    ret
