#include "idt.h"
struct regs;
void irq_install();
void irq_install_handler(int irq, void (*handler)(struct regs *r));
