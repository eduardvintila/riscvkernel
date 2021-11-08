#!/bin/sh
set -e
. ./build.sh

qemu-system-riscv64 -M virt -m 652M -nographic -bios firmware/fw_jump.bin -kernel $SYSROOT/boot/test.kernel
