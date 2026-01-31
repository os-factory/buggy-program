
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
static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
    /* There's an outb %al, $imm8 encoding, for compile-time constant port numbers that fit in 8b. (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}
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
unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};		
void keyboard_handler(struct regs *r)
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */
        writechar(kbdus[scancode], 15);
    }
}

void kernel_main() {
	clear();
	gdt_install();
	idt_install();
	irq_install();
	irq_install_handler(1, keyboard_handler);
	__asm__ __volatile__ ("sti"); 
	writestring("Installed the GDT.\n", 2);
	writestring("Installed the IDT.\n", 2);
	writestring("Installed the IRQs.\n", 2);
	writestring("Info: version 3.11a, Filename: main.kernel\n", 15);
}
