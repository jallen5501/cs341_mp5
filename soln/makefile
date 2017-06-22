# makefile for building cs341 'tutor' programs using the
# Standalone 486 or Pentium IBM PC running in 32-bit protected mode,
# or "SAPC" for short.  UNIX builds also, and "make clean"

# system directories needed for compilers, libraries, header files--
# assumes the environment variables SAPC_TOOLS and SAPC_GNUBIN
# have been set up by the ulab module

PC_LIB = $(SAPC_TOOLS)/lib
PC_INC = $(SAPC_TOOLS)/include

PROJDIR = /home/bobw/cs341/mp5

# We require warning free ansi compatible C:
#
CFLAGS = -g -Wall -Wstrict-prototypes -Wmissing-prototypes \
		-Wno-uninitialized -Wshadow -ansi \
		-D__USE_FIXED_PROTOTYPES__ \
		-I$(PROJDIR)

# for SAPC builds
PC_CC   = $(SAPC_GNUBIN)/i386-gcc
PC_CFLAGS = $(CFLAGS) -I$(PC_INC)
PC_AS   = $(SAPC_GNUBIN)/i386-as
PC_LD   = $(SAPC_GNUBIN)/i386-ld
PC_NM   = $(SAPC_GNUBIN)/i386-nm

# File suffixes:
# .c	C source (often useful both for UNIX and SAPC)
# .s 	assembly language source (gnu as for protected mode 486)
# .o    relocatable machine code, initialized data, etc., for UNIX
# .opc  relocatable machine code, initialized data, etc., for SAPC
# .lnx  executable image (bits as in memory), for SA PC (Linux a.out format)
# .syms text file of .exe's symbols and their values (the "symbol table")
# .usyms text file of UNIX executable's symbols

PC_OBJS = $(PROJDIR)/tutor.opc $(PROJDIR)/slex.opc tickpack.opc comintspack.opc
UNIX_OBJS = $(PROJDIR)/tutor.o $(PROJDIR)/slex.o


# PC executable--tell ld to start code at 0x1000e0, load special startup
# module, special PC C libraries--
# Code has 0x20 byte header, so use "go 100100"
all: tutor.lnx test_tickpack.lnx

test_tickpack.lnx: test_tickpack.opc tickpack.opc
	$(PC_LD) -N -Ttext 1000e0 -o test_tickpack.lnx \
		$(PC_LIB)/startup0.opc $(PC_LIB)/startup.opc \
		test_tickpack.opc tickpack.opc $(PC_LIB)/libc.a
	rm -f syms;$(PC_NM) -n test_tickpack.lnx>test_tickpack.syms; \
		ln -s test_tickpack.syms syms

tickpack.opc: tickpack.c tickpack.h
	$(PC_CC) $(PC_CFLAGS) -c -o tickpack.opc tickpack.c

test_tickpack.opc: test_tickpack.c tickpack.h
	$(PC_CC) $(PC_CFLAGS) -c -o test_tickpack.opc test_tickpack.c

comintspack.opc: comintspack.c comintspack.h
	$(PC_CC) $(PC_CFLAGS) -c -o comintspack.opc comintspack.c

tutor.lnx: cmds.opc $(PC_OBJS) \
		$(PC_LIB)/startup0.opc $(PC_LIB)/startup.opc $(PC_LIB)/libc.a
	$(PC_LD) -N -Ttext 1000e0 -o tutor.lnx \
		$(PC_LIB)/startup0.opc $(PC_LIB)/startup.opc \
		$(PC_OBJS) cmds.opc $(PC_LIB)/libc.a
	rm -f syms;$(PC_NM) -n tutor.lnx>tutor.syms;ln -s tutor.syms syms

# this rule allows you to build your own cmds.opc--
cmds.opc: cmds.c $(PROJDIR)/slex.h
	$(PC_CC) $(PC_CFLAGS) -c -o cmds.opc cmds.c

# these should already have been made for you--
$(PROJDIR)/tutor.opc: $(PROJDIR)/tutor.c $(PROJDIR)/slex.h
	$(PC_CC) $(PC_CFLAGS) -c -o $(PROJDIR)/tutor.opc $(PROJDIR)/tutor.c

$(PROJDIR)/slex.opc: $(PROJDIR)/slex.c $(PROJDIR)/slex.h
	$(PC_CC) $(PC_CFLAGS) -c -o $(PROJDIR)/slex.opc $(PROJDIR)/slex.c

# ************** UNIX build **********
# tell make to use gcc as C compiler, -g -W... for C compiler flags--
# check "man gcc" to find out what -W flags do
CC = gcc

tutor:  $(UNIX_OBJS) cmds.o
	$(CC) -g -o tutor $(UNIX_OBJS) cmds.o
	rm -f usyms; nm -n tutor > tutor.usyms; ln -s tutor.usyms usyms

# make knows to use $(CC) and $(CFLAGS)
cmds.o:	cmds.c  $(PROJDIR)/slex.h
$(PROJDIR)/tutor.o: $(PROJDIR)/tutor.c $(PROJDIR)/slex.h
$(PROJDIR)/slex.o: $(PROJDIR)/slex.c  $(PROJDIR)/slex.h
# **************end of UNIX build ********

clean:
	rm -f *.o *.opc *.syms *.usyms *.lnx tutor core syms usyms

spotless: clean
	rm -f *~
