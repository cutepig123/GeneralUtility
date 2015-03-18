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

PLASMA_DIR = ..
##include ../Makefile.internal
include make.inc

all: lib

lib: 	libquark.a

clean:
	rm -f *.o *~ 

cleanall: clean
	rm -f *.a 

quark.o: quark.c quark.h bsd_tree.h bsd_queue.h

icl_hash.o: icl_hash.c icl_hash.h

icl_list.o: icl_list.c icl_list.h

libquark.a: icl_hash.o icl_list.o quarkos.o quark.o 
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

install: lib
	mkdir -p $(prefix) $(prefix)/include $(prefix)/lib
	cp ./quark.h ./quark_unpack_args.h $(prefix)/include
	cp ./libquark.a $(prefix)/lib

uninstall: 
	- rm -f $(prefix)/include/quark.h $(prefix)/include/quark_unpack_args.h 
	- rm -f $(prefix)/lib/libquark.a


HWLOC = $(shell pkg-config --libs hwloc 2> /dev/null )
ifeq (1, $(words $(findstring hwloc, $(HWLOC))))
	CFLAGS  := $(CFLAGS) -DQUARK_HWLOC
	INC     := $(INC) $(shell pkg-config --cflags-only-I hwloc)
	LIBEXT  := $(LIBEXT) $(shell pkg-config --libs hwloc)
	require := hwloc
endif


.PHONY: clean

