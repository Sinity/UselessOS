#include "io.h"

#define SCREEN_HEIGHT 25
#define SCREEN_WIDTH 80
#define SCREEN_PTR (char*)0xB8000

struct {
	unsigned int x;
	unsigned int y;
} cursor = {0, 0};

BackgroundColor bgColor = BG_BLACK;
ForegroundColor fgColor = FG_WHITE;

void newLine() {

}

void kputs(const char* str) {
	const char* c;
	for(c = str; *c != 0; c++)
		kputc(*c);
}

void kputc(char c) {
	if(cursor.x == SCREEN_WIDTH) {
		if(cursor.y == SCREEN_HEIGHT) {
			//currently i don't know if, and how I will handle it here.
		}
		else {
			cursor.x = 0;
			cursor.y++;
		}
	}

	if(c == '\n') {
		cursor.x = 0;
		cursor.y++;
	} 
	else {
		unsigned int offset = (cursor.y*SCREEN_WIDTH + cursor.x)*2;
		*(SCREEN_PTR + offset) = c;
		*(SCREEN_PTR + offset + 1) = fgColor | bgColor;
		cursor.x++;		
	}
}

void resetCursor(void) {
	cursor.x = 0;
	cursor.y = 0;
}

void setColor(char color) {
	fgColor = color & 0xF;
	bgColor = color - fgColor;
} 