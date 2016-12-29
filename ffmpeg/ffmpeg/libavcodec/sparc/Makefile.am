INCLUDES = \
	-I$(top_srcdir)/libavutil \
	-I$(top_srcdir)/libavcodec \
	-DHAVE_AV_CONFIG_H=1

noinst_LTLIBRARIES = libsparc.la

libsparc_la_SOURCES = \
	dsputil_vis.c

noinst_HEADERS = \
	vis.h

libsparc_la_CFLAGS = \
	-mcpu=ultrasparc \
	-mtune=ultrasparc
