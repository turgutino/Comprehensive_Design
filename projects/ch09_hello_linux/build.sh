#!/bin/sh
# Cross-compile the Section 9 program with the bare-metal toolchain that is
# already installed for the other chapters (no separate mipsel-linux-gcc needed).
# Produces a static ELF32 MIPS little-endian executable with the o32 ABI.
set -e
mips-sde-elf-gcc -EL -march=mips32 -mabi=32 -mno-abicalls -fno-pic -O1 \
    -nostdlib -nostartfiles -static \
    -Wl,-e,__start -Wl,-Ttext=0x00400000 \
    -Wl,--build-id=none -Wl,-z,max-page-size=0x1000 \
    -o hello_linux hello_linux.c
mips-sde-elf-readelf -h hello_linux | head -12
