INCLUDES = \
	-I$(top_srcdir)/libavutil \
	-I$(top_srcdir)/libavcodec \
	-DHAVE_AV_CONFIG_H=1

if HAVE_IWMMXT
iwmmxt_libs = libiwmmxt.la
endif

noinst_LTLIBRARIES = \
	libarmv4l.la \
	$(iwmmxt_libs)

libarmv4l_la_SOURCES = \
	jrevdct_arm.S \
	simple_idct_arm.S \
	dsputil_arm_s.S \
	dsputil_arm.c \
	mpegvideo_arm.c

libiwmmxt_la_SOURCES = \
	dsputil_iwmmxt.c \
	mpegvideo_iwmmxt.c

noinst_HEADERS = \
	dsputil_iwmmxt_rnd.h
