#include "gdt.h"

#pragma pack(1)
struct gdtr {
	uint16_t size;
	void* base;
};
#pragma pack()

static struct gdt_descriptor gdt[MAX_DESCRIPTORS];
struct gdtr	gdtr;

void loadGDT(void);

void setGDTDescriptor(uint32_t i, uint32_t base, uint32_t limit, uint8_t flags, uint8_t grand)
{
	if (i > MAX_DESCRIPTORS) //todo: assert here
		return;

    for(unsigned int i = 0; i < sizeof(struct gdt_descriptor); i++)
        ((char*)&gdt[i])[i] = 0;

	gdt[i].baseLow = (uint16_t)base & 0xffff;
	gdt[i].baseMid = (uint8_t)((base >> 16) & 0xff);
	gdt[i].baseHigh = (uint8_t)((base >> 24) & 0xff);
	gdt[i].limitLow = (uint16_t)limit & 0xffff;

    gdt[i].flags = flags;
	gdt[i].granularity = (uint8_t)((limit >> 16) & 0x0f);
	gdt[i].granularity |= grand & 0xf0;
}

struct gdt_descriptor* getGDTDescriptor(int i) {
	if (i > MAX_DESCRIPTORS)
		return 0;

	return &gdt[i];
}

int initializeGDT(void) {
    gdtr.size = (sizeof(struct gdt_descriptor) * MAX_DESCRIPTORS) - 1;
	gdtr.base = gdt;

	//null descriptor
    setGDTDescriptor(0, 0, 0, 0, 0);

	// code descriptor
    setGDTDescriptor(
		1,
		0,
		0xffffffff,
		GDT_NOACCESS | 
        GDT_READWRITE |
		GDT_NOCONFORMING | 
		GDT_CODE | 
		GDT_CODEDATA_DESC | 
		GDT_RING0 | 
		GDT_SEGMENT_IN_MEMORY,
        GDT_GRANULARITY_4K |
		GDT_32BIT | 
        GDT_LIMITHIGH_MASK
	);

	// data descriptor
    setGDTDescriptor(
		2,
		0,
		0xffffffff,
		GDT_NOACCESS | 
        GDT_READWRITE |
		GDT_NOCONFORMING | 
		GDT_DATA | 
		GDT_CODEDATA_DESC | 
		GDT_RING0 | 
		GDT_SEGMENT_IN_MEMORY,
        GDT_GRANULARITY_4K |
		GDT_32BIT | 
        GDT_LIMITHIGH_MASK
	);

    loadGDT();
	return 0;
}

