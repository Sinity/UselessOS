[bits 32]

extern gdtr

global loadGDT
loadGDT:
    lgdt [gdtr]
    ret
