#include "console_output.h"
#include <size_t.h>
#include <stdarg.h>

struct VideoChar {
	char ascii;
	char attribute;
};

#define VIDEOMEM_PTR ((struct VideoChar*)0xB8000)
static struct VideoChar* screen = VIDEOMEM_PTR;

static struct {
	int8_t x;
	int8_t y;
} cursor = {0, 0};

static BackgroundColor bgColor = defaultBGColor;
static ForegroundColor fgColor = defaultFGColor;
int8_t blinking = 0; 





void kputs(const char* str) {
	while(*str != 0)
		kputc(*str++);
}

void kSetChar(char c, int16_t x, int8_t y) {
	screen[y*SCREEN_WIDTH + x].ascii = c;
	screen[y*SCREEN_WIDTH + x].attribute = bgColor | fgColor | blinking;
}

void kputc(char c) {
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

	//todo: bell, check if all these control codes works well
	switch(c) {
		case '\n':
			cursor.y++;
		case '\r':
			cursor.x = 0;
			break;

		case '\b':
			if(cursor.x != 0)
				screen[cursor.y*SCREEN_WIDTH + cursor.x].ascii = c;
				--cursor.x;
			break;

		case '\f':
			clearScreen();
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

void newLine(void) {
	kputc('\n');
}

void scrollUp(void) {
	for(int16_t i = SCREEN_WIDTH; i < SCREEN_WIDTH*SCREEN_HEIGHT; i++)
		screen[i-SCREEN_WIDTH] = screen[i];  
	
	for(int16_t i = (SCREEN_HEIGHT-1)*SCREEN_WIDTH; i < SCREEN_HEIGHT*SCREEN_WIDTH; i++) {
		screen[i].ascii = 0x20;
		screen[i].attribute = bgColor | blinking;
	}
}

void clearScreen(void) {
	for(int16_t i = 0; i < SCREEN_HEIGHT*SCREEN_WIDTH; i++) {
		screen[i].ascii = 0x20;
		screen[i].attribute = bgColor | blinking;
	}
	resetCursor();
}





void resetCursor(void) {
	cursor.x = 0;
	cursor.y = 0;
}

void setCursor(int8_t x, int8_t y) {
	cursor.x = x;
	cursor.y = y;
}





void setColor(int8_t color) {
	fgColor = color & 0xF;
	bgColor = color - fgColor;
} 

void setBGColor(int8_t color) {
	bgColor = color;
}

void setFGColor(int8_t color) {
	fgColor = color;
}

void enableBlinking(void) { 
	blinking = 1 << 7;
}

void disableBlinking(void) {
	blinking = 0;
}





void kput(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	#define ESCAPE_CHAR '%'

	for(int32_t i = 0; fmt[i] != '\0'; i++) {
		if(fmt[i] == ESCAPE_CHAR) {
			switch(fmt[++i]) {
				case ESCAPE_CHAR:
					kputc(ESCAPE_CHAR);
					break;
				case 'd':
					kputint(va_arg(args, int32_t));
					break;
				case 'u':
					kputuint(va_arg(args, uint32_t));
					break;
				case 'b':
					kputbool(va_arg(args, uint8_t));
					break;
				case 'h':
					kputhex(va_arg(args, uint32_t));
					break;
				case 'p':
					kputptr(va_arg(args, void*));
					break;
				case 'c':
					kputc(va_arg(args, char));
					break;
				case 's':
					kputs(va_arg(args, const char*));
					break;

				default: 	//todo: better solution for this(panic?) 
					kput("\n(KPUT ERROR: wrong character code: \"%c\")\n", fmt[i]);
					break;
			}
		} else {
			kputc(fmt[i]);
		}
	}

	va_end(args);
}





static uint32_t abs(int32_t value);
static size_t utoa(uint32_t integer, char* dst, int8_t base);
static size_t itoa(int32_t integer, char* dst, int8_t base);

#define BASE_DEC 10
#define BASE_HEX 16

void kputint(int32_t integer) {
	char string[12];
	itoa(integer, string, BASE_DEC);
	kputs(string);
}

void kputuint(uint32_t integer) {
	char string[11];
	utoa(integer, string, BASE_DEC);
	kputs(string);
}

void kputhex(uint32_t integer) {
	char string[10];
	utoa(integer, string, BASE_HEX);
	kputs("0x"); 
	kputs(string);
}

void kputptr(void* ptr) {
	char string[9];
	size_t len = utoa((uint32_t)ptr, string, BASE_HEX);

	kputs("0x");
	for(size_t i = 8 - len; i != 0; i--)
		kputc('0');
	kputs(string);
}

void kputbool(uint8_t boolean) {
	if(boolean == 0)
		kputs("false");
	else
		kputs("true");
}





static size_t utoa(uint32_t integer, char* dst, int8_t base) {
	//todo: assert if base is in range(2, 16)(because base 1 would need 4gb of memory :S 
	//and I don't know how to represent numbers higher than F) (firstly I must develop assert).
	static const char digits[16] = "0123456789ABCDEF";
	char invertedInteger[32];

	size_t digitsNumber = 0;

	if(integer == 0) {
		dst[0] = '0';
		dst[1] = '\0';
		return 1;
	}

	size_t i;
	for(i = 0; integer > 0; integer /= base) {
		invertedInteger[i++] = digits[integer % base];
	}
	digitsNumber += i;

	dst[digitsNumber] = '\0';
	for(size_t j = 0; j < digitsNumber; j++)
		dst[j] = invertedInteger[--i];

	return digitsNumber;
}

static size_t itoa(int32_t integer, char* dst, int8_t base) {
	//todo: assert if base is in range(2, 16)(because base 1 would need 4gb of memory :S 
	//and I don't know how to represent numbers higher than F) (firstly I must develop assert).
	static const char digits[16] = "0123456789ABCDEF";
	char invertedInteger[32];

	size_t digitsNumber = 0;
	int8_t negative = 0;
	uint32_t arg = abs(integer);

	if(integer < 0) {
		digitsNumber++;
		negative = 1;
		dst[0] = '-';
	} else if(integer == 0) {
		dst[0] = '0';
		dst[1] = '\0';
		return 1;
	}

	size_t i;
	for(i = 0; arg != 0; arg /= base) {
		invertedInteger[i++] = digits[arg % base];
	}
	digitsNumber += i;

	dst[digitsNumber] = '\0';
	for(size_t j = negative; j < digitsNumber; j++)
		dst[j] = invertedInteger[--i];

	return digitsNumber;
}

uint32_t abs(int32_t val) {
	if(val < 0)
		return -val;
	else
		return val;
}
