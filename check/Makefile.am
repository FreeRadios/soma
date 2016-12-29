bin_PROGRAMS = somacheck

EXTRA_DIST = check.h

somacheck_SOURCES = main.c fatal.c time.c palinsesto.c spot.c host.c

if ENABLE_SM
sm = ../sm/libsm.a
endif

if ENABLE_CONFUSE
confuse = ../confuse/libconfuse.a
endif

somacheck_LDADD = $(confuse) $(sm)

