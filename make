#!/usr/bin/bash

#make bootloader
nasm bootloader/first.asm -f bin -o build/bootsector.bin
nasm bootloader/second.asm -f bin -o build/secondStage.bin

#make kernel
i586-elf-gcc -c kernel/main.c -o build/main.o -std=gnu99 -ffreestanding -Wall -Wextra -I./include -O2
i586-elf-gcc -c kernel/console_output.c -o build/console_output.o -std=gnu99 -ffreestanding -Wall -Wextra -I./include -O2 

#make hal
i586-elf-gcc -c hal/hal.c -o build/hal.o -std=gnu99 -ffreestanding -Wall -Wextra -I./include -O2
i586-elf-gcc -c hal/gdt.c -o build/gdt.o -std=gnu99 -ffreestanding -Wall -Wextra -I./include -O2
i586-elf-gcc -c hal/idt.c -o build/idt.o -std=gnu99 -ffreestanding -Wall -Wextra -I./include -O2
nasm hal/idt.asm -f elf -o build/idt_asm.o
nasm hal/gdt.asm -f elf -o build/gdt_asm.o

i586-elf-gcc -T kernel/linker.ld -o build/KRNL.SYS -ffreestanding -nostdlib build/*.o -lgcc -O2

rm build/*.o
