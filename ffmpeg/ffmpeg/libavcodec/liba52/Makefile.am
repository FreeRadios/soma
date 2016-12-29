INCLUDES = \
	-I$(top_srcdir)/libavutil \
	-I$(top_srcdir)/libavcodec \
	-I$(top_srcdir)/libavcodec/libpostproc

noinst_LTLIBRARIES = liba52.la

liba52_la_SOURCES = \
	bit_allocate.c \
	bitstream.c \
	crc.c \
	downmix.c \
	imdct.c \
	parse.c \
	resample.c \
	resample_c.c \
	resample_mmx.c
liba52_la_LDFLAGS = \
	$(M_LIBS)

noinst_HEADERS = \
	a52.h \
	a52_internal.h \
	a52_util.h \
	bitstream.h \
	mm_accel.h \
	tables.h
