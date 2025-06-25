# dsblocks version
VERSION = 1.0

# paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

# includes and libs
LIBS = -lX11 -lasound -lm

# flags
CFLAGS = -std=c99 -pedantic -Wall -Os
LDFLAGS = $(LIBS)

# compiler and linker
CC = cc
