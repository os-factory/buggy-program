
#include <stdint.h>
#include <stddef.h>
#include "gdt.h"
#include "idt.h"
#include "print.h"
#include "isrs.h"
#include "irq.h"

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

char sbase20[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K'
};

char sbase16[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

char sbase10[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

char sbase2[] = {
    '0', '1'
};

void int_to_string(int Integer, char *Destination, char Array[], int Division) {
    int i = 0;

    if (Integer == 0) {
        Destination[i++] = '0';
    }

    while (Integer > 0) {
        Destination[i++] = Array[Integer % Division];
        Integer /= Division;
    }

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char Temp = Destination[j];
        Destination[j] = Destination[k];
        Destination[k] = Temp;
    }

    Destination[i] = '\0';
}
 

void writechar(char Letter, uint16_t Attribute2) {
	if(Letter == '\n') {
		// TODO: Check if y is equal to Height and scroll for the next line.
		y++;
		x = 0;
		return;
	}
	
	array[y * Width + x] = write(Letter, Attribute2);
	x++;
}
void clear(void) {
	uint8_t terminal_color = setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	for (uint8_t y = 0; y < Height; y++) {
		for (uint8_t x = 0; x < Width; x++) {
			const uint8_t index = y * Width + x;
			array[index] = write(' ', terminal_color);
		}
	}
}


void writestring(char text[], uint16_t color3) {
    for (int i = 0; i < strlen(text); i++) {
        writechar(text[i], color3);
    }
}

void kernel_main() {
	clear();
	gdt_install();
	idt_install();
	irq_install();
	writestring("Installed the GDT.\n", 2);
	writestring("Installed the IDT.\n", 2);
	writestring("Info: version 3.1a, Filename: main.kernel", 15);
}
