#!/bin/sh

cat > /etc/apt/sources.list <<EOL
deb http://cdn.debian.net/debian jessie main contrib non-free
deb-src http://cdn.debian.net/debian jessie main contrib non-free

deb http://archive.ev3dev.org/debian jessie main
deb-src http://archive.ev3dev.org/debian jessie main
EOL
apt-get update
apt-get build-dep brickman
apt-get install symlinks
symlinks -c /usr/lib/arm-linux-gnueabi
