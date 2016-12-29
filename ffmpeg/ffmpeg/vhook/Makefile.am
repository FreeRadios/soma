INCLUDES = \
	-I$(top_srcdir)/libavutil \
	-I$(top_srcdir)/libavformat \
	-I$(top_srcdir)/libavcodec \
	-DHAVE_AV_CONFIG_H=1 \
	$(GPROF_FLAGS) \
	$(WARNERR_FLAGS)

LDADD = \
	$(GPROF_FLAGS) \
	$(WARNERR_FLAGS)

AM_LDFLAGS = \
	$(M_LIBS) \
	-module \
	-avoid-version

if HAVE_FREETYPE2
freetype2libs = libdrawtext.la
endif

if HAVE_IMLIB2
imlib2libs = libimlib2.la
endif

vhookdir = $(libdir)/vhook
vhook_LTLIBRARIES = \
	libnull.la \
	libfish.la \
	libppm.la \
	$(freetype2libs) \
	$(imlib2libs)

libnull_la_SOURCES = null.c
libfish_la_SOURCES = fish.c
libppm_la_SOURCES = ppm.c

libdrawtext_la_SOURCES = drawtext.c
libdrawtext_la_CFLAGS = $(FT2_CFLAGS)
libdrawtext_la_LIBADD = $(FT2_LIBS)

libimlib2_la_SOURCES = imlib2.c
libimlib2_la_CFLAGS = $(IMLIB_CFLAGS)
libimlib2_la_LIBADD = $(IMLIB_LIBS)
