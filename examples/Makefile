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

