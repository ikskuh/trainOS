#pragma once

#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25

#define COLOR_BLACK 0x0
#define COLOR_BLUE 0x1
#define COLOR_GREEN 0x2
#define COLOR_CYAN 0x3
#define COLOR_RED 0x4
#define COLOR_MAGENTA 0x5
#define COLOR_BROWN 0x6
#define COLOR_LIGHTGRAY 0x7
#define COLOR_GRAY 0x8
#define COLOR_LIGHTBLUE 0x9
#define COLOR_LIGHTGREEN 0xA
#define COLOR_LIGHTRED 0xC
#define COLOR_LIGHTMAGENTA 0xD
#define COLOR_YELLOW 0xE
#define COLOR_WHITE 0xF

/**
 * @brief Sets the cursor position.
 * @param x Distance of the cursor from the left border.
 * @param y Distance of the cursor from the top border.
 */
void ksetpos(int x, int y);


/**
 * @brief Gets the cursor position.
 * @param x Pointer to an integer that should store the distance of the cursor from the left border.
 * @param y Pointer to an integer that should store the distance of the cursor from the top border.
 */
void kgetpos(int *x, int *y);

/**
 * @brief Sets the color of the console.
 * @param foreground The foreground color.
 * @param background The background color.
 */
void ksetcolor(int background, int foreground);


/**
 * @brief Gets the color of the console.
 * @param foreground A pointer to the foreground color.
 * @param background A pointer to the background color.
 */
void kgetcolor(int *background, int *foreground);

/**
 * @brief Clears the screen and sets the cursor to the top left corner.
 */
void kclear(void);

/**
 * @brief Puts a character onto the screen.
 * @param c The character that should be printed.
 */
void kputc(char c);

/**
 * @brief Prints a string onto the screen.
 * @param A null-terminated string that should be printed.
 */
void kputs(const char *str);

/**
 * @brief Prints a formatted string onto the screen.
 * @param format The format string that will be printed in formatted version.
 * @param ... The format parameters that will be used to print the string.
 */
void kprintf(const char *format, ...);