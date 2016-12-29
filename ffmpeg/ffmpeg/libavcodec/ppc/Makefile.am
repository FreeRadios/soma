INCLUDES = \
	-I$(top_srcdir)/libavutil \
	-I$(top_srcdir)/libavcodec \
	-DHAVE_AV_CONFIG_H=1

if HAVE_ALTIVEC
altivec_libs = libaltivec.la
endif

noinst_LTLIBRARIES = \
	libppc.la \
	$(altivec_libs)

libppc_la_SOURCES = \
	dsputil_ppc.c \
	mpegvideo_ppc.c

noinst_HEADERS = \
	dsputil_ppc.h \
	gcc_fixes.h \
	dsputil_altivec.h \
	dsputil_h264_template_altivec.c

libaltivec_la_SOURCES = \
	dsputil_altivec.c \
	mpegvideo_altivec.c \
	idct_altivec.c \
	fft_altivec.c \
	gmc_altivec.c \
	fdct_altivec.c \
	dsputil_h264_altivec.c

libppc_la_CFLAGS = $(ALTIVEC_CFLAGS)

libaltivec_la_CFLAGS = $(ALTIVEC_CFLAGS)
