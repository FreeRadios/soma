bin_PROGRAMS = somaclient

EXTRA_DIST = client.h

somaclient_SOURCES = cmd.c main.c other.c rl.c shell.c time.c

if ENABLE_SM
sm = ../sm/libsm.a
endif

if ENABLE_CONFUSE
confuse = ../confuse/libconfuse.a
endif

somaclient_LDADD = $(confuse) $(sm) ../lib/libsoma.la

