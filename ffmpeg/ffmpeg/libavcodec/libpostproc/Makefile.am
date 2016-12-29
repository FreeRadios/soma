INCLUDES = \
	-I$(top_srcdir)/libavutil \
	-I$(top_srcdir)/libavcodec \
	$(ALTIVEC_CFLAGS) \
	-fomit-frame-pointer

pp_sources = \
	postprocess.c
pp_libs = \
	$(M_LIBS)

if SHARED_PP
lib_LTLIBRARIES = libpostproc.la
libpostproc_la_SOURCES = $(pp_sources)
libpostproc_la_LDFLAGS = $(pp_libs)
inst_hdrs = postprocess.h
else
noinst_LTLIBRARIES = libffpostproc.la
libffpostproc_la_SOURCES = $(pp_sources)
libffpostproc_la_LDFLAGS = $(pp_libs)
endif

postprocdir = $(includedir)/postproc
postproc_HEADERS = $(inst_hdrs)

noinst_HEADERS = \
	mangle.h \
	postprocess_internal.h \
	postprocess_template.c \
	postprocess_altivec_template.c \
	postprocess.h
