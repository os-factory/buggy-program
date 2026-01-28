#include <stdint.h>
#include <stddef.h>
#include "gdt.h"


uint8_t x = 0;
uint8_t y = 0;
const uint8_t Width = 80;
const uint8_t Height = 25;
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
uint16_t write(uint16_t chara, uint16_t color) {
    return chara | color << 8;
}
uint16_t* array = (uint16_t*)0xB8000;
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t setcolor(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
void writechar(char arg, uint16_t color2) {
if(arg == '\n') {}
array[y * Width + x] = write(arg, color2);
x++;
}
void init(void) {
      uint8_t terminal_color = setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        for (uint8_t y = 0; y < Height; y++) {
                for (uint8_t x = 0; x < Width; x++) {
                const uint8_t index = y * Width + x;
		array[index] = write(' ', terminal_color);
                }
        }
}


void string(char text[], uint16_t color3) {
    for (int i = 0; i < strlen(text); i++) {
        writechar(text[i], color3);
    }
}

void kernel_main() {
init();
gdt_install();
string("Installed the GDT.\n", 2);
string("Nothing to do.", 15);
}
