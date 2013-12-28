#ifndef __GDT_H_
#define __GDT_H_

#include <stdint.h>

#define MAX_DESCRIPTORS 3

/*** flags ***/

#define GDT_ACCESS 0x0001
#define GDT_NOACCESS 0x0000

#define GDT_READWRITE 0x0002
#define GDT_READONLY 0x0000

#define GDT_EXPANSION 0x0004
#define GDT_CONFORMING 0x0004
#define GDT_NOEXPANSION 0x0000
#define GDT_NOCONFORMING 0x0000

#define GDT_CODE 0x0008
#define GDT_DATA 0x0000

#define GDT_SYSTEM_DESC 0x0000
#define GDT_CODEDATA_DESC 0x0010

#define GDT_RING0 0x0000
#define GDT_RING3 0x0060

#define GDT_SEGMENT_IN_MEMORY 0x0080
#define GDT_SEGMENT_NOT_IN_MEMORY 0x0000

#define GDT_LIMITHIGH_MASK 0x0f // mask out limitHigh bits which are contained in granularity byte

#define GDT_OSBIT_ON 0x10
#define GDT_OSBIT_OFF 0x00

#define GDT_16BIT 0x00
#define GDT_32BIT 0x40

#define GDT_GRANULARITY_0K 0x00
#define GDT_GRANULARITY_4K 0x80


#pragma pack(1)
struct gdt_descriptor {
    uint16_t		limitLow;

	uint16_t		baseLow;
	uint8_t			baseMid;

	// descriptor various flags & bits 16-20 of segment limit
	uint8_t			flags;
	uint8_t			granularity;

	// bits 24-32 of base address
	uint8_t			baseHigh;
};
#pragma pack()

int initializeGDT(void);

struct gdt_descriptor* getGDTDescriptor(int i);
void setGDTDescriptor(uint32_t i, uint32_t base, uint32_t limit, uint8_t flags, uint8_t grand);

#endif
