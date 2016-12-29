INCLUDES = \
	-I$(top_srcdir)/libavcodec \
	-DHAVE_AV_CONFIG_H=1 \
	$(GPROF_FLAGS) \
	$(WARNERR_FLAGS) \
	$(ALTIVEC_CFLAGS)

LDADD = \
	$(GPROF_FLAGS) \
	$(WARNERR_FLAGS)

noinst_LTLIBRARIES = libavutil.la
libavutil_la_SOURCES = \
	eval.c \
	integer.c \
	intfloat_readwrite.c \
	mathematics.c \
	rational.c

libavutildir = $(includedir)/ffmpeg
libavutil_HEADERS = \
	avutil.h \
	bswap.h \
	common.h \
	mathematics.h \
	rational.h \
	integer.h \
	intfloat_readwrite.h
