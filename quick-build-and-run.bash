#!/bin/bash

./build.bash

qemu-system-x86_64 -cpu qemu64,+la57 -bios /usr/share/OVMF/x64/OVMF.fd -drive file=build/diskimage.img,if=ide

