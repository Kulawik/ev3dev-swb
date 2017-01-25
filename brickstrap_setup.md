**Note** -- All scripts used in this document are also present in the directory ./scripts in this repository.

1. Install Ubuntu 16.04
2. Run [install_brickstrap.sh](scripts/install_brickstrap.sh) script

    ```sh
sudo apt-key adv --keyserver pgp.mit.edu --recv-keys 2B210565
sudo apt-add-repository "deb http://archive.ev3dev.org/ubuntu trusty main"
sudo apt-get update
sudo apt-get install -y brickstrap

# create a supermin appliance
sudo update-guestfs-appliance
# add yourself to the kvm group
# need to log out and back in for this to take effect
sudo usermod -a -G kvm $USER
newgrp kvm
# fix permissions on /boot/vmlinuz*
sudo chmod +r /boot/vmlinuz*
# And you need to add yourself to /etc/subuid and /etc/subgid to be able to use uid/gid mapping.
sudo usermod --add-subuids 200000-265534 --add-subgids 200000-265534 $USER

# create virtual environment
mkdir work
cd work
 -p argument from ls /usr/share/brickstrap/projects
 -c argument form ls /usr/share/brickstrap/projects/ev3dev-jessie
brickstrap -p ev3dev-jessie -c ev3 -d ev3-rootfs create-rootfs
brickstrap -d ev3-rootfs shell << EOF
apt-get update
apt-get install -y build-essential
EOF
```
3. Setup gdbserver support (TODO Maciek)
4. Install cross-compilation environment with cmake 3.x:
    * Install CMake 3.5:

        ```sh
    sudo apt-get install cmake
    ```
    * Run install_deps_in_shell.sh script inside brickstrap shell:

        ```sh
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
    ```

    * Leave brickstrap shell and run [prepare_cmake_env.sh](scripts/prepare_cmake_env.sh):

        ```sh
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
    export PKG_CONFIG_LIBDIR="$PKG_CONFIG_LIBDIR:\
    $SYSROOT_PATH/usr/lib/pkgconfig"
    export PKG_CONFIG_LIBDIR="$PKG_CONFIG_LIBDIR:\
    $SYSROOT_PATH/usr/share/pkgconfig"
    export PKG_CONFIG_LIBDIR="$PKG_CONFIG_LIBDIR:\
    $SYSROOT_PATH/usr/local/lib/arm-linux-gnueabi/pkgconfig"
    export PKG_CONFIG_LIBDIR="$PKG_CONFIG_LIBDIR:\
    $SYSROOT_PATH/usr/local/lib/pkgconfig"
    export PKG_CONFIG_LIBDIR="$PKG_CONFIG_LIBDIR:\
    $SYSROOT_PATH/usr/local/share/pkgconfig"

    export XDG_DATA_DIRS=${SYSROOT_PATH}/usr/local/share:${SYSROOT_PATH}/usr/share
    EOL
    ```

    * Run

        ``` sh
    source ./ev3-rootfs-cross.env
    ```


5. Configure makefile-based cross compilation environment:
    * Create makefile (example in ./examples):

        ```make
    PROGRAM = my-program
    CROSS_COMPILE = arm-linux-gnueabi-
    SYSROOT = $(HOME)/work/ev3-rootfs/rootfs

    CC=$(CROSS_COMPILE)gcc
    LD=$(CROSS_COMPILE)ld
    CFLAGS= --sysroot=$(SYSROOT) -g -I$(SYSROOT)/usr/include

    all: $(PROGRAM)

    LIBDIR = -L=/usr/lib/arm-linux-gnueabi
    #LIBDIR = -L$(SYSROOT)/usr/lib/arm-linux-gnueabi

    LIBS = -lpthread

    LDFLAGS= $(LIBDIR) $(LIBS)
    SOURCE = my_program.c

    OBJS = $(SOURCE:.c=.o)

    $(PROGRAM): $(OBJS)
        $(CC) -o $@ $(OBJS) $(LDFLAGS)

    clean:
        rm -f $(OBJS) $(PROGRAM)
    ```

    * Use the makefile to compile the project
