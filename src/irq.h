#include "idt.h"
void irq_install();
void irq_install_handler(int irq, void (*handler)(struct regs *r));
