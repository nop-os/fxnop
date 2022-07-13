#!/usr/bin/sh

sh-elf-gcc $(find . -name "*.c") -m4-nofpu -Os -s -Iinclude -c
sh-elf-ld $(find . -name "*.o") -L/usr/lib/gcc/sh3eb-elf/11.2.0 -lgcc -nostdlib -T linker.ld -o fxnop.elf

rm -f $(find . -name "*.o")

sh-elf-objcopy -O binary fxnop.elf fxnop.bin
fxg1a --internal="@FXNOP" --name="fxnop" fxnop.bin -o ../fxnop.g1a

rm fxnop.bin
