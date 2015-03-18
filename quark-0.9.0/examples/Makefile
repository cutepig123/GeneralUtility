##
#
# @file Makefile
#
# QUARK is a software package provided by Univ. of Tennessee,
# Univ. of California Berkeley and Univ. of Colorado Denver
#
# @version 2.4.5
# @author Asim YarKhan
# @date 2010-11-15
#
###

# Pick up compilers and linker from QUARK
include ../make.inc

QUARK_DIR ?= ..
QUARK_CFLAGS ?= -I${QUARK_DIR}
QUARK_LDLIBS ?= ${QUARK_DIR}/libquark.a

CFLAGS += ${QUARK_CFLAGS}
LDLIBS += ${QUARK_LDLIBS}

all: exec_all

EXEC = 

EXEC += hello_world
hello_world: hello_world.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

EXEC += matmul_example
matmul_example: matmul_example.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:	
	rm -f *.o 

cleanall: clean
	rm -f *.a $(EXEC) 

exec_all: ${EXEC}

.PHONY: clean
