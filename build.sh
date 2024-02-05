#!/bin/bash
set -eu

# build efi image
cd edk2
set +u
# shellcheck disable=SC1091
source edksetup.sh
set -u
build
cp Build/MikanLoaderX64/DEBUG_CLANGPDB/X64/Loader.efi MikanLoaderPkg
cd ..

# build kernel image
# shellcheck disable=SC1091
source osbook/devenv/buildenv.sh
cd kernel
# shellcheck disable=SC2086,SC2154,SC2250
clang++ $CPPFLAGS -O2 --target=x86_64-elf -fno-exceptions -ffreestanding  -c main.cpp
# shellcheck disable=SC2086,SC2154,SC2250
ld.lld $LDFLAGS --entry KernelMain -z norelro --image-base 0x100000 --static -o kernel.elf main.o
cd ..
