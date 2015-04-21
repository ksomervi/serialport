# Makefile 
#

HOST=$(shell uname -o)

CXXFLAGS = -Wall -g -O3 

ifeq "$(HOST)" "Cygwin"
CC = i686-pc-mingw32-gcc
CXX = i686-pc-mingw32-g++

CXXFLAGS += -DWIN32

LDFLAGS = -static-libstdc++ -static-libgcc
FL_LDFLAGS = -mwindows -lfltk -lole32 -luuid -lcomctl32 $(LDFLAGS)

else
CC = gcc
CXX = g++
AR = ar

AR_FLAGS = cvq
LDFLAGS = -static-libgcc
endif

CFLAGS = $(CXXFLAGS) -std=c99 

CM_SRC = communication_manager.cpp serial_port.cpp
CM_OBJ = $(subst cpp,o,$(CM_SRC))

TARGETS = libcm.a

.SILENT:

.cpp.o:
	echo "# Compiling $@ ..."
	$(CXX) $(CXXFLAGS) $(INCDIRS) -c $<

.PHONY: default all neat clean

default: libcm.a

all: $(TARGETS)

libcm.a: $(CM_OBJ)
	echo "# Linking $@ ..."
	$(AR) $(AR_FLAGS) $@ $(CM_OBJ)

neat: 
	-rm -f *~ *.o

clean: neat
	echo "# Cleaning up ${TARGETS} ..."
	-rm -f ${TARGETS}

