#include "idt.h"

#pragma pack(1)
struct idtr {
	uint16_t size;
	void* base;
};
#pragma pack()

static struct idt_descriptor idt[MAX_INTERRUPTS];
struct idtr	idtr;


int setInterrupt(uint8_t i, uint8_t flags, uint16_t sel, IRQ_HANDLER irq) {
    if(!irq)
		return 0; //todo: assert here

    uint32_t uiBase = (uint32_t)&(*irq);

    idt[i].baseLow = (uint16_t)uiBase & 0xffff;
    idt[i].baseHigh	= (uint16_t)((uiBase >> 16) & 0xffff);
    idt[i].reserved	= 0;
    idt[i].flags = flags;
	idt[i].codeSelector	= sel;

    return 0;
}

void loadIDT(void);
int initializeIDT(uint16_t codeSel, IRQ_HANDLER defaultHandler) {
    idtr.size = sizeof(struct idt_descriptor) * MAX_INTERRUPTS -1;
	idtr.base = idt;

    for(unsigned int i = 0; i < sizeof(struct idt_descriptor) * MAX_INTERRUPTS - 1; i++)
        *((char*)idt + i) = 0;

	// register default handlers
	for (int i = 0; i < MAX_INTERRUPTS; i++)
        setInterrupt(i, IDT_DESC_PRESENT | IDT_32BIT | IDT_RING0, codeSel, defaultHandler);

    loadIDT();
	return 0;
}

struct idt_descriptor* getInterrupt(uint8_t i) {
    return &idt[i];
}
