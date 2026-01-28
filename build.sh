mkdir build
mkdir result
ls build
i686-elf-gcc -c src/kernel.c -o build/kernel.o \
-ffreestanding -O2 -std=gnu99

i686-elf-gcc -c src/gdt.c -o build/gdt.o \
-ffreestanding -O2 -std=gnu99
i686-elf-gcc -c src/idt.c -o build/idt.o \
-ffreestanding -O2 -std=gnu99
i686-elf-gcc -c src/isrs.c -o build/isrs.o \
-ffreestanding -O2 -std=gnu99

i686-elf-as src/boot.s -o build/boot.o
i686-elf-as src/gdtflush.s -o build/gdtflush.o 
i686-elf-as src/idt.s -o build/idts.o
i686-elf-as src/isrs.s -o build/isrss.o
i686-elf-gcc -nostdlib -T src/linker.ld \
-o result/main.kernel \
build/boot.o build/kernel.o build/gdt.o build/gdtflush.o build/idt.o build/idts.o build/isrs.o build/isrss.o \
-lgcc
