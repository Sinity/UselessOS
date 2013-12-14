#include "console_output.h"

void kmain(void) {
	setFGColor(FG_GREEN);
	kputs("UselessOS Kernel\n");
	
	for(;;); //halt
}
