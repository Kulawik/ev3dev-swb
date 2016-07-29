#!/bin/sh

sudo apt-get install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi cmake valac pkg-config
cd ~/work
cat > arm-linux-gnueabi.cmake <<EOL
set(CMAKE_SYSROOT $ENV{HOME}/work/ev3-rootfs/rootfs)

set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
#set(CMAKE_CXX_COMPILER arm-linux-gnueabi-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOL
cat > ev3-rootfs-cross.env <<EOL
SYSROOT_PATH=${HOME}/work/ev3-rootfs/rootfs

export PKG_CONFIG_ALLOW_SYSTEM_CFLAGS=1
export PKG_CONFIG_ALLOW_SYSTEM_LIBS=1
export PKG_CONFIG_SYSROOT_DIR=${SYSROOT_PATH}
export PKG_CONFIG_LIBDIR=${SYSROOT_PATH}/usr/lib/arm-linux-gnueabi/pkgconfig
export PKG_CONFIG_LIBDIR=${PKG_CONFIG_LIBDIR}:${SYSROOT_PATH}/usr/lib/pkgconfig
export PKG_CONFIG_LIBDIR=${PKG_CONFIG_LIBDIR}:${SYSROOT_PATH}/usr/share/pkgconfig
export PKG_CONFIG_LIBDIR=${PKG_CONFIG_LIBDIR}:${SYSROOT_PATH}/usr/local/lib/arm-linux-gnueabi/pkgconfig
export PKG_CONFIG_LIBDIR=${PKG_CONFIG_LIBDIR}:${SYSROOT_PATH}/usr/local/lib/pkgconfig
export PKG_CONFIG_LIBDIR=${PKG_CONFIG_LIBDIR}:${SYSROOT_PATH}/usr/local/share/pkgconfig

export XDG_DATA_DIRS=${SYSROOT_PATH}/usr/local/share:${SYSROOT_PATH}/usr/share
EOL
