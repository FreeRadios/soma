if HAVE_VHOOK
vhook_DIR = vhook
vhook_libs = $(DL_LIBS)
endif

SUBDIRS = \
	doc \
	libavutil \
	libavcodec \
	libavformat \
	tests \
	$(vhook_DIR)

DIST_SUBDIRS = \
	doc \
	libavutil \
	libavcodec \
	libavformat \
	tests \
	vhook

INCLUDES = \
	-I$(srcdir)/libavutil \
	-I$(srcdir)/libavcodec \
	-I$(srcdir)/libavformat \
	$(GPROF_FLAGS) \
	$(WARNERR_FLAGS)

LIBS = \
	libavutil/libavutil.la \
	libavformat/libavformat.la \
	libavcodec/libavcodec.la

AM_LDFLAGS = \
	$(GPROF_FLAGS) \
	$(WARNERR_FLAGS) \
	$(vhook_LIBS) \
	$(GETOPTLONG_LIBS)

if CONFIG_FFPLAY
ffplay_PROG = ffplay
endif

if CONFIG_FFSERVER
ffserver_PROG = ffserver
endif

if CONFIG_FFMPEG
ffmpeg_PROG = ffmpeg
endif

if CONFIG_ENCODERS
output_PROG = output_example
endif

noinst_PROGRAMS = \
	$(ffmpeg_PROG) \
	$(ffplay_PROG) \
	$(ffserver_PROG) \
	$(output_PROG)

ffmpeg_SOURCES = \
	cmdutils.c \
	ffmpeg.c

ffplay_SOURCES = \
	cmdutils.c \
	ffplay.c
ffplay_CFLAGS = $(SDL_CFLAGS)
ffplay_LDFLAGS = $(SDL_LIBS)

ffserver_SOURCES = ffserver.c
ffserver_LDFLAGS = $(DL_LIBS)

output_example_SOURCES = output_example.c

noinst_HEADERS = \
	cmdutils.h \
	ffserver.h

#if CONFIG_PP
#pp_pc_files = libpostproc.pc
#endif
#
#pcfilesdir = $(libdir)/pkgconfig
#pcfiles_DATA = \
#	libavcodec.pc \
#	libavformat.pc \
#	$(pp_pc_files)

EXTRA_DIST = \
	Changelog \
	COPYING \
	CREDITS \
	INSTALL \
	README
#	libavcodec.pc \
#	libavformat.pc \
#	libpostproc.pc \
#	libavcodec-uninstalled.pc \
#	libavformat-uninstalled.pc \
#	libpostproc-uninstalled.pc
