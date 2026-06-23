#!/bin/bash
JA_LIB_DIR="/usr/.ja/lib"
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"

mkdir -p "$JA_LIB_DIR"
cp "$SCRIPT_DIR/libag.so" "$JA_LIB_DIR/"
chmod 755 "$JA_LIB_DIR/libag.so"

echo "[JackAndroid] libag.so → /usr/.ja/lib/"

