#!/bin/sh
# Build mipsel-linux-gnu binutils (assembler/linker) when the distro package is unavailable.
# usage: tools/build-binutils.sh [PREFIX]   (default $HOME/.local/mipsel)
set -e
PREFIX=${1:-$HOME/.local/mipsel}
VER=2.40
WORK=$(mktemp -d)
cd "$WORK"
curl -fsSLO https://ftp.gnu.org/gnu/binutils/binutils-$VER.tar.xz
tar xf binutils-$VER.tar.xz
mkdir build && cd build
../binutils-$VER/configure --target=mipsel-linux-gnu --prefix="$PREFIX" --disable-nls --disable-gdb --disable-werror
make -j"$(nproc)"
make install
echo "installed to $PREFIX/bin (add to PATH)"
