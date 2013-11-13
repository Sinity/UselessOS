#include "io.h"

void kmain(void) {
	setColor(FG_GREEN);
	kputs("UselessOS Kernel\n");

	for(;;); //halt
}
