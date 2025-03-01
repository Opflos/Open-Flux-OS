#!/bin/bash

mkdir -p /work/OpenFlux-OS/iso/boot/grub


nasm -f bin -o /work/OpenFlux-OS/boot/boot.bin /work/OpenFlux-OS/boot/boot.asm

# ملاحظة هنا مسار كل ملف حطه :)
gcc -ffreestanding -c /work/OpenFlux-OS/kernel/kernel.c -o /work/OpenFlux-OS/kernel/kernel.o
gcc -ffreestanding -c /work/OpenFlux-OS/kernel/ui.c -o /work/OpenFlux-OS/kernel/ui.o
ld -o /work/OpenFlux-OS/kernel/kernel.bin -Ttext 0x1000 --oformat binary /work/OpenFlux-OS/kernel/kernel.o /work/OpenFlux-OS/kernel/ui.o


cat /work/OpenFlux-OS/boot/boot.bin /work/OpenFlux-OS/kernel/kernel.bin > /work/OpenFlux-OS/iso/boot/myos.bin
cp /work/OpenFlux-OS/iso/grub.cfg /work/OpenFlux-OS/iso/boot/grub/
grub-mkrescue -o /work/OpenFlux-OS/myos.iso /work/OpenFlux-OS/iso


rm -rf /work/OpenFlux-OS/iso/boot/myos.bin /work/OpenFlux-OS/boot/boot.bin /work/OpenFlux-OS/kernel/*.o /work/OpenFlux-OS/kernel/kernel.bin

echo "تم إنشاء ملف ISO بنجاح: /work/OpenFlux-OS/myos.iso"
