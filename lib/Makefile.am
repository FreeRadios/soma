AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libsoma.la

EXTRA_DIST = libsoma.h

if !ENABLE_FFMPEG
INCLUDES = $(CFLAGS) \
	-I$(top_srcdir)/ffmpeg/ffmpeg/libavformat \
	-I$(top_srcdir)/ffmpeg/ffmpeg/libavcodec \
	-I$(top_srcdir)/ffmpeg/ffmpeg/libavutil
endif

libsoma_la_SOURCES = \
	filesystem.c \
	main.c \
	palinsesto.c \
	spot.c \
	init.c \
	nextitem.c \
	simple.c \
	stat.c \
	lists.c \
	local.c

libsoma_la_LDFLAGS = -version-info 2:0

if !ENABLE_FFMPEG
libsoma_la_LIBADD = \
	$(top_builddir)/ffmpeg/ffmpeg/libavformat/libavformat.la
endif

