#!/bin/bash

OUTDIR="../rootfs/usr/bin/"

echo "Compiling Riot utils..."
sudo mkdir -p ./build

for src in *.c; do
	bin="${src%.c}"
	echo "Compiling $src -> $OUTDIR/$bin"

	if [ "$src" = "rlogin.c" ] || [ "$src" = "adduser.c" ]; then
		sudo gcc -std=c11 "$src" -o "build/$bin" -lcrypt
	else
		sudo gcc -static -O2 "$src" -o "build/$bin"
	fi
done
echo "Done!"
echo "Copying all binaries to rootfs"
sudo cp ./build/* "$OUTDIR"
echo "Done!"
