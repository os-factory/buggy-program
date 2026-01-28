mkdir build
mkdir result
i686-elf-as src/boot.s -o build/boot.o
i686-elf-gcc -c src/kernel.c src/gdtflush.s src/gdt.c -o build/kernel.o -std=gnu99 -ffreestanding -O2
i686-elf-gcc -T src/linker.ld -o result/main.kernel -ffreestanding -O2 -nostdlib build/boot.o build/kernel.o -lgcc
