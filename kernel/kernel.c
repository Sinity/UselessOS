#include "io.h"
void hltProcessor(void);

void kmain(void) {
	setColor(FG_GREEN);
	const char* greet = "Hello, user! Kernel here :)";
	kputs(greet);

	hltProcessor();
}

void hltProcessor(void) {
	__asm__("cli \n\
			hlt");
}
