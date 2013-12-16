#ifndef __CONSOLE_OUTPUT_H
#define __CONSOLE_OUTPUT_H

#include <stdint.h>

/* basic console output */
void kSetChar(char c, int16_t x, int8_t y); /* puts character on screen in desired position. For linear offset give it to x and set y to 0*/ 
void kputc(char c); /* put char on screen */
void kputs(const char* str); /* put zero-terminated string on screen */
void kput(const char* fmt, ...); /* printf-like function */
void newLine(void); /* puts new line on screen */
void scrollUp(void); /* moves each character to the upper line. Destroys first line. */
void clearScreen(void); /* throws out all chars from screen and sets background color to current background color */

/* output common data types */
void kputint(int32_t integer);
void kputuint(uint32_t integer);
void kputhex(uint32_t integer);
void kputptr(void* ptr);
void kputbool(uint8_t boolean);

/* attributes */
void setColor(int8_t color); /* sets color mode of next characters */
void setBGColor(int8_t color); /* sets background color of next characters */
void setFGColor(int8_t color); /* sets text color of next characters */
void enableBlinking(void);
void disableBlinking(void);

/* default colors */
#define defaultFGColor FG_WHITE
#define defaultBGColor BG_BLACK

/* cursor position */
void setCursor(int8_t x, int8_t y); /* sets cursor to given coordinates */ 
void resetCursor(void); /* sets cursor to top-right corner of screen */ 

/* screen size */
#define SCREEN_HEIGHT 25
#define SCREEN_WIDTH 80

/*color of characters on screen */
typedef enum { 
	FG_BLACK = 0,
	FG_BLUE,
	FG_GREEN,
	FG_CYAN,
	FG_RED,
	FG_MAGENTA,
	FG_BROWN,
	FG_LIGHT_GRAY,
	FG_DARK_GRAY,
	FG_LIGHT_BLUE,
	FG_LIGHT_GREEN,
	FG_LIGHT_CYAN,
	FG_LIGHT_RED,
	FG_LIGHT_MAGENTA,
	FG_LIGHT_BROWN,
	FG_WHITE = 0xF
} ForegroundColor; 

/*color of background of screen */
typedef enum { 
	BG_BLACK  = 0,
	BG_BLUE   = 2 << 3,
	BG_GREEN  =	2 << 4,
	BG_CYAN   = BG_BLUE + BG_GREEN,
	BG_RED 	  = 2 << 5,
	BG_PURPLE = BG_BLUE + BG_RED,
	BG_BROWN  = BG_GREEN + BG_RED,
	BG_GRAY   = BG_BLUE + BG_GREEN + BG_RED 
} BackgroundColor; 

#endif
