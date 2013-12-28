#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

#define MAX_INTERRUPTS 256

#define IDT_16BIT 0x06
#define IDT_32BIT 0x0E

#define IDT_RING0 0x00
#define IDT_RING3 0x60

#define IDT_DESC_PRESENT 0x80

typedef void (__attribute__((cdecl)) *IRQ_HANDLER)(void);

#pragma pack(1)
struct idt_descriptor {
	uint16_t		baseLow;
	
	uint16_t		codeSelector;
	
	uint8_t			reserved;
	uint8_t			flags;

    uint16_t		baseHigh;
};
#pragma pack()

int initializeIDT(uint16_t codeSelector, IRQ_HANDLER defaultHandler);

struct idt_descriptor* getInterrupt(uint8_t i);
int setInterrupt(uint8_t i, uint8_t flags, uint16_t codeSelector, IRQ_HANDLER);

#endif
