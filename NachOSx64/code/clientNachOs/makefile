# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as
#GCCDIR = ~/gnu/local/decstation-ultrix/bin/
GCCDIR = ../bin/
LDFLAGS = -T script -N
ASFLAGS = -mips1
CPPFLAGS = $(INCDIR)


# if you aren't cross-compiling:
#GCCDIR =
#LDFLAGS = -N -T 0
#ASFLAGS =
#CPPFLAGS = -P $(INCDIR)

PATH := ../bin:$(PATH)

CC = $(GCCDIR)mips-gcc
AS = $(GCCDIR)as
LD = $(GCCDIR)mips-ld

CPP = /lib/cpp
INCDIR =-I../userprog -I../threads
# Corrects the error: dangerous relocation GP
CFLAGS = -c $(INCDIR) -mips1 -G 0


all: client

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	/bin/rm strt.s
	
client.o: client.c
	$(CC) $(CFLAGS) -c client.c
client: client.o start.o
	$(LD) $(LDFLAGS) start.o client.o -o client.coff
	../bin/coff2noff client.coff client




# Estas reglas sirven para compilar programas simples, que consistan en un unico fuente
# Las reglas anteriores para construir los ejecutables
# halt, shell, sort y matmult se podrian suprimir

%: %.c 
	make start.o 
	$(CC) $(CFLAGS) -c $<
	$(LD) $(LDFLAGS) start.o $*.o -o $*.coff
	../bin/coff2noff $*.coff $@
	

