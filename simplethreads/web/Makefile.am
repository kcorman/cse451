bin_PROGRAMS = sioux

ldadd = ../lib/libsthread.la
AM_LDFLAGS = ../lib/sthread_start.o

INCLUDES = -I ../include

sioux_SOURCES = sioux.c sioux_run.c web_queue.c
sioux_LDADD = $(ldadd)

noinst_HEADERS = sioux_run.h web_queue.h

EXTRA_DIST = docs/index.html webclient
