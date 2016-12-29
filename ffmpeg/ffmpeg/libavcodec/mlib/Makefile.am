INCLUDES = \
	-I$(top_srcdir)/libavutil \
	-I$(top_srcdir)/libavcodec \
	-DHAVE_AV_CONFIG_H=1

noinst_LTLIBRARIES = libmlib.la

libmlib_la_SOURCES = dsputil_mlib.c
libmlib_la_CFLAGS = $(MLIB_INC)
libmlib_la_LDFLAGS = -lmlib
