#
# Makefile for lab1 in Process Oriented Programming
#
# Course: Operating Systems and Multicore Programming - OSM
# Lab assignment 1: Game of luck
#
# Author: Nikos Nikoleris <nikos.nikoleris@it.uu.se>
#

# change to y to enable debugging support
DEBUG=n

CFLAGS=-Wall -Wextra
LDFLAGS=
LDLIBS=

CC=gcc

ifeq ($(DEBUG), y)
	CFLAGS += -g 
else
	CFLAGS += -O2
endif

all: craps shooter

craps: craps.o common.o

shooter: shooter.o common.o

clean:
	$(RM) *~ *.o *.d

distclean:
	$(RM) craps shooter

pack: clean
	echo tar -czvf ${USER}-lab1-sumbit.tar.gz *.c *.h Makefile

.PHONY: all clean pack

%.o: %.c
	gcc -c $(CFLAGS) $*.c -o $*.o
	gcc -MM $(CFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

-include $(SRCS:%.c=%.d)
