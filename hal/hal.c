#include <hal.h>
#include <stdint.h>

#include "gdt.h"
#include "idt.h"

void initializeHal(void) {
    initializeGDT();

    const uint16_t codeSelector = 0x08;
    void __attribute__((cdecl)) defaultInterruptHandler(void);
    initializeIDT(codeSelector, defaultInterruptHandler);
}

uint8_t IRQ[3] = { 0xCD, 0x00, 0xC3 };
void generateInterrupt(uint8_t n) {
    IRQ[1] = n;
    ( (void (*)(void))IRQ )();
}

// default handler to catch unhandled system interrupts.
#include "../kernel/console_output.h"
void __attribute__((cdecl)) defaultInterruptHandler(void) {
    setColor(FG_WHITE | BG_BLUE);
    clearScreen();
    setCursor(CENTER_X - 15, CENTER_Y);
    kputs("[HAL_x86] UNHANDLED INTERRUPT!");
    for(;;); //todo: assert here
}
