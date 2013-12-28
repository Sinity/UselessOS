#include <hal.h>
#include "console_output.h"

void kmain(void) {
    initializeHal();

    setFGColor(FG_GREEN);
	kputs("UselessOS Kernel\n");

	for(;;); //halt
}
