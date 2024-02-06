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
make
cd ..
