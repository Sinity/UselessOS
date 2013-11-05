void hltProcessor(void);

void kputc(char c, char attrib);
#define PUTWC(c) kputc(c, 15)

void kputs(const char* str, char attrib);
#define PUTWS(str) kputs(str, 15)

void resetCursor(void);

#define SCREEN_HEIGHT 25
#define SCREEN_WIDTH 80
#define SCREEN_PTR (char*)0xB8000

struct Cursor {
	unsigned int x;
	unsigned int y;
};

struct Cursor cursor;

void kmain(void) {
	resetCursor();
	const char* greet = "Hello, user! Kernel here :)";
	PUTWS(greet);

	hltProcessor();
}


void kputs(const char* str, char attrib) {
	const char* c;
	for(c = str; *c != 0; c++)
		kputc(*c, attrib);
}

void kputc(char c, char attrib) {
	if(c == '\n') {
		(cursor).x = 0;
		(cursor).y++;
		return;
	}

	unsigned int offset = ((cursor).y*SCREEN_WIDTH + (cursor).x)*2;
	*(SCREEN_PTR + offset) = c;
	*(SCREEN_PTR + offset + 1) = attrib;
	(cursor).x++;

	if((cursor).x == 80) {
		(cursor).x = 0;
		(cursor).y++;	
	}			
}

void resetCursor(void) {
	(cursor).x = 0;
	(cursor).y = 0;
}

void hltProcessor(void) {
	__asm__("cli\n\t"
			"hlt");
}
