#!/usr/bin/bash

#make bootloader
nasm bootloader/first.asm -f bin -o build/bootsector.bin
nasm bootloader/second.asm -f bin -o build/secondStage.bin

#make kernel
i586-elf-gcc -c kernel/main.c -o build/main.o -std=gnu99 -ffreestanding -Wall -Wextra -I./include -O2
i586-elf-gcc -c kernel/console_output.c -o build/console_output.o -std=gnu99 -ffreestanding -Wall -Wextra -I./include -O2 

i586-elf-gcc -T kernel/linker.ld -o build/KRNL.SYS -ffreestanding -nostdlib build/*.o -lgcc -O2

rm build/*.o
