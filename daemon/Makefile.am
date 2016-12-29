bin_PROGRAMS = somad

EXTRA_DIST = somad.h

somad_SOURCES = \
	build.c \
	echo.c \
	list_item.c \
	log.c \
	module.c \
	palinsesto.c \
	play.c \
	signal.c \
	start.c \
	data.c \
	file.c \
	list_pl.c \
	main.c \
	options.c \
	pid.c \
	socket.c \
	nextitem.c \
	distribuited.c \
	spot.c \
	playlist.c \
	denyallow.c \
	stat.c \
	string.c

if ENABLE_SM
sm = $(top_builddir)/sm/libsm.a
endif

if ENABLE_CONFUSE
confuse = $(top_builddir)/confuse/libconfuse.a
endif

if !ENABLE_FFMPEG
INCLUDES = $(CFLAGS) \
	-I$(top_srcdir)/ffmpeg/ffmpeg/libavformat \
	-I$(top_srcdir)/ffmpeg/ffmpeg/libavcodec \
	-I$(top_srcdir)/ffmpeg/ffmpeg/libavutil
endif


somad_LDADD = \
        $(confuse) \
	$(sm) \
	$(top_builddir)/ghthash/libghthash.a

if !ENABLE_FFMPEG
somad_LDADD += \
	$(top_builddir)/ffmpeg/ffmpeg/libavformat/libavformat.la \
	$(top_builddir)/ffmpeg/ffmpeg/libavcodec/libavcodec.la \
	$(top_builddir)/ffmpeg/ffmpeg/libavutil/libavutil.la
endif

