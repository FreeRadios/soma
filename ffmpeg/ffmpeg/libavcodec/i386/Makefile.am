INCLUDES = \
	-I$(top_srcdir)/libavutil \
	-I$(top_srcdir)/libavcodec \
	-DHAVE_AV_CONFIG_H=1 \
	-fomit-frame-pointer \
	$(WARNERR_FLAGS)

noinst_LTLIBRARIES = libmmxsse.la

libmmxsse_la_SOURCES = \
	fdct_mmx.c \
	cputest.c \
	dsputil_mmx.c \
	mpegvideo_mmx.c \
	idct_mmx.c \
	idct_mmx_xvid.c \
	motion_est_mmx.c \
	simple_idct_mmx.c \
	fft_sse.c \
	vp3dsp_mmx.c \
	vp3dsp_sse2.c

if HAVE_BUILTIN_VECTOR
libmmxsse_la_CFLAGS = -msse
endif

tests_pass = cputest
noinst_PROGRAMS = $(tests_pass)
cputest_CFLAGS = -D__TEST__

noinst_HEADERS = \
	mpegvideo_mmx_template.c \
	dsputil_mmx_avg.h \
	dsputil_mmx_rnd.h \
	mmx.h \
	h264dsp_mmx.c \
	dsputil_h264_template_mmx.c
