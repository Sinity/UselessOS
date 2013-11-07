void kputc(char c); /* put char on screen */
void kputs(const char* str); /* put zero-terminated string on screen */

void setColor(char color); /* sets color mode of next characters */
void resetCursor(void); /* sets cursor to top-right corner of screen */ 

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