#include "io.h"

#define SCREEN_PTR (char*)0xB8000

struct {
	unsigned int x;
	unsigned int y;
} cursor = {0, 0};

BackgroundColor bgColor = BG_BLACK;
ForegroundColor fgColor = FG_WHITE;
unsigned char blinking = 0; 

void kputs(const char* str) {
	while(*str != 0)
		kputc(*str++);
}

void kputc(char c) {
	//check screen overflows
	if(cursor.y == SCREEN_HEIGHT) {
		scrollUp();
		--cursor.y;
	}
	if(cursor.x == SCREEN_WIDTH) {
		cursor.x = 0;
		if(cursor.y == SCREEN_HEIGHT-1) {
			scrollUp();
		} else {
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
		*(SCREEN_PTR + offset + 1) = fgColor | bgColor | blinking;
		cursor.x++;		
	}
}

void resetCursor(void) {
	cursor.x = 0;
	cursor.y = 0;
}

void setCursor(unsigned int x, unsigned int y) {
	cursor.x = x;
	cursor.y = y;
}

void setColor(char color) {
	fgColor = color & 0xF;
	bgColor = color - fgColor;
} 

void enableBlinking() { 
	blinking = 1 << 7;
}

void disableBlinking() {
	blinking = 0;
}

void scrollUp(void) {
	for(char* i = SCREEN_PTR + SCREEN_WIDTH*2; i < SCREEN_PTR + SCREEN_WIDTH*SCREEN_HEIGHT*2; i++) {
		*(i-SCREEN_WIDTH*2) = *i;  
	}
}