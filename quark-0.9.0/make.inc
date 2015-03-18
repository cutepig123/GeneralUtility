###
#
# @file Makefile
#
#  PLASMA is a software package provided by Univ. of Tennessee,
#  Univ. of California Berkeley and Univ. of Colorado Denver
#
# @version 2.4.5
# @author Asim YarKhan
# @date 2010-11-15
#
###

prefix=./install
CC=cc
CFLAGS=-O2
AR=ar 
ARFLAGS=cr
RANLIB=ranlib
LD=${CC}
LDLIBS=-lpthread

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
