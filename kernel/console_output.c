#include "console_output.h"

struct VideoChar {
	char ascii;
	char attribute;
};
#define VIDEOMEM_PTR ((struct VideoChar*)0xB8000)
struct VideoChar* screen = VIDEOMEM_PTR;

struct {
	unsigned int x;
	unsigned int y;
} cursor = {0, 0};

BackgroundColor bgColor = defaultBGColor;
ForegroundColor fgColor = defaultFGColor;
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
	if(cursor.x >= SCREEN_WIDTH) {
		cursor.x = 0;
		if(cursor.y == SCREEN_HEIGHT-1) {
			scrollUp();
		} else {
			cursor.y++;
		}
	}

	switch(c) {
		case '\n':
			cursor.y++;
		case '\r':
			cursor.x = 0;
			break;

		case '\b':
			if(cursor.x != 0)
				--cursor.x;
			break;

		case '\t':
			cursor.x = (cursor.x+8) & ~(8-1);
			break;

		default:
			screen[cursor.y*SCREEN_WIDTH + cursor.x].ascii = c;
			screen[cursor.y*SCREEN_WIDTH + cursor.x].attribute = fgColor | bgColor | blinking;
			cursor.x++;
	}
}

void scrollUp(void) {
	for(unsigned int i = SCREEN_WIDTH; i < SCREEN_WIDTH*SCREEN_HEIGHT; i++)
		screen[i-SCREEN_WIDTH] = screen[i];  
	
	for(unsigned int i = (SCREEN_HEIGHT-1)*SCREEN_WIDTH; i < SCREEN_HEIGHT*SCREEN_WIDTH; i++) {
		screen[i].ascii = 0x20;
		screen[i].attribute = bgColor | blinking;
	}
}

void clearScreen(void) {
	for(unsigned int i = 0; i < SCREEN_HEIGHT*SCREEN_WIDTH; i++) {
		screen[i].ascii = 0x20;
		screen[i].attribute = bgColor | blinking;
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

void setBGColor(char color) {
	bgColor = color;
}

void setFGColor(char color) {
	fgColor = color;
}

void enableBlinking(void) { 
	blinking = 1 << 7;
}

void disableBlinking(void) {
	blinking = 0;
}

