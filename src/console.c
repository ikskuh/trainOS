#include "console.h"
#include "stdlib.h"

typedef enum
{
	csDefault = 0,
	csSetColor,
	csSetForeground,
	csSetBackground,
} ConsoleState;

typedef struct
{
	int foreground:4;
	int background:4;
} ConsoleColor;

typedef struct
{
	char c;
	uint8_t color;
} ConsoleChar;

static int cursorX = 0;
static int cursorY = 0;
static ConsoleColor color = { COLOR_LIGHTGRAY, COLOR_BLACK };
static ConsoleState state = csDefault;

static ConsoleChar *screen = (ConsoleChar*)0xB8000;

static ConsoleChar *statusbar = (ConsoleChar*)(0xB8000 + 0x02 * CONSOLE_WIDTH * CONSOLE_HEIGHT);

void console_setstate(const char *text)
{
    for(size_t i = 0; i < CONSOLE_WIDTH; i++) {
        statusbar[i].c = ' ';
        statusbar[i].color = 0x70;
    }
    ConsoleChar *ptr = statusbar;
    while(*text) {
        (ptr++)->c = *text++;
    }

}

void ksetpos(int x, int y)
{
	if((x >= 0) && (x < CONSOLE_WIDTH))
	{
		cursorX = x;
	}
	
	if((y >= 0) && (y < CONSOLE_HEIGHT))
	{
		cursorY = y;
	}
}

void kgetpos(int *x, int *y)
{
	if(x != nullptr) *x = cursorX;
	if(y != nullptr) *y = cursorY;
}


void ksetcolor(int background, int foreground)
{
	if(background >= 0)
	{
		background &= 0xF;
		color.background = background;
	}
	if(foreground >= 0)
	{
		foreground &= 0xF;
		color.foreground = foreground;
	}
}

void kgetcolor(int *background, int *foreground)
{
	if(background != nullptr) *background = color.background;
	if(foreground != nullptr) *foreground = color.foreground;
}

void kclear(void)
{
	cursorX = 0;
	cursorY = 0;
	for(size_t i = 0; i < (CONSOLE_WIDTH * CONSOLE_HEIGHT); i++)
	{
		screen[i].c = ' '; // Fill screen with spaces
		screen[i].color = *((uint8_t*)&color); // Set screen to current color
	}
}

static void newline()
{
	cursorX = 0;
	cursorY += 1;
	if(cursorY == (CONSOLE_HEIGHT))
	{
		// We need to scroll here:
		
		// Copy everything line on the screen to the previous line
		for(int line = 1; line < CONSOLE_HEIGHT; line++)
		{
			memcpy(&screen[CONSOLE_WIDTH * (line-1)], &screen[CONSOLE_WIDTH * line], CONSOLE_WIDTH * sizeof(ConsoleChar));
		}
		// Fill the last line with blanks
		for(int i = 0; i < CONSOLE_WIDTH; i++)
		{
			screen[CONSOLE_WIDTH * (CONSOLE_HEIGHT-1)+i].c = ' ';
			screen[CONSOLE_WIDTH * (CONSOLE_HEIGHT-1)+i].color = *((uint8_t*)&color);
		}
		// Reset y cursor position to previous line (we don't want to write out of bounds)
		cursorY -= 1;
	}
}

void kputc(char c)
{
	switch(state)
	{
		case csSetColor:
		{
			color = *((ConsoleColor*)&c);
			state = csDefault;
			return;
		}
		case csSetForeground:
		{
			color.foreground = c & 0xF;
			state = csDefault;
			return;
		}
		case csSetBackground:
		{
			color.background = c & 0xF;
			state = csDefault;
			return;
		}
		case csDefault:
		default:
		{
			int idx = CONSOLE_WIDTH * cursorY + cursorX;
			switch(c)
			{
			case '\x11':
				state = csSetColor;
				return;
			case '\x12':
				state = csSetForeground;
				return;
			case '\x13':
				state = csSetBackground;
				return;
			case '\r':
				// Ignore carriage return
				return; 
			case '\n':
				newline(); 
				return;
			default:
				screen[idx].color = *((uint8_t*)&color); // Set screen to current color
				screen[idx].c = c;
				
				cursorX += 1;
				if(cursorX >= CONSOLE_WIDTH)
				{
					newline();
				}
				return;
			}
			break;
		}
	}
}

void kputs(const char *str)
{
	while(*str)
	{
		kputc(*str);
		str++;
	}
}

void kprintf(const char *format, ...)
{
	static char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	va_list vl;
	va_start(vl, format);
	vsprintf(buffer, format, vl);
	va_end(vl);
	kputs(buffer);
}
