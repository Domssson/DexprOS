#!/bin/bash

dd if=/dev/zero of=build/diskimage.img bs=1k count=2880
mformat -i build/diskimage.img -f 2880 ::
mmd -i build/diskimage.img ::/EFI
mmd -i build/diskimage.img ::/EFI/BOOT
mcopy -i build/diskimage.img build/BOOTX64.EFI ::/EFI/BOOT
