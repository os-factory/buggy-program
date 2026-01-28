mkdir build
mkdir result
ls build
i686-elf-as src/boot.s -o build/boot.o
i686-elf-gcc  src/kernel.c src/gdt.c src/gdtflush.s -o kernel.o -std=gnu99 -ffreestanding -O2
i686-elf-gcc -T src/linker.ld -o result/main.kernel -ffreestanding -O2 -nostdlib build/boot.o build/kernel.o  -lgcc
