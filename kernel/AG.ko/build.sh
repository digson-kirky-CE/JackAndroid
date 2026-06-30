#!/bin/bash
# JackAndroid AG.ko Build Script
# Kernel source at ~/kernel
# Author: digson-kirky-CE

set -e

KDIR="$HOME/kernel"
CURDIR="$(pwd)"

echo "============================================"
echo " JackAndroid - by digson-kirky-CE"
echo " Building AG.ko + libag.so (AeroGlass)"
echo "============================================"

# Step 1: 编译 libag.so
echo ""
echo "[1/2] Building libag.so..."
cd lib
gcc -shared -fPIC -Wall -Wl,-soname,libag.so -o libag.so libag.c
ls -lh libag.so
cd ..

# Step 2: 编译 AG.ko
echo ""
echo "[2/2] Building AG.ko..."
cd "$CURDIR"
make -C "$KDIR" M="$CURDIR" modules
ls -lh AG.ko

echo ""
echo "============================================"
echo " ✅ Build complete!"
echo "    sudo insmod AG.ko  to install"
echo "============================================"

