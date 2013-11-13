#!/usr/bin/bash

#make bootloader
nasm bootloader/first.asm -f bin -o build/bootsector.bin
nasm bootloader/second.asm -f bin -o build/secondStage.bin

#make kernel
i586-elf-gcc -c kernel/main.c -o build/main.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i586-elf-gcc -c kernel/io.c -o build/io.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

i586-elf-gcc -T kernel/linker.ld -o build/KRNL.SYS -ffreestanding -O2 -nostdlib build/*.o -lgcc

rm build/*.o
