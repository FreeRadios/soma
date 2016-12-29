INCLUDES = \
	-DHAVE_AV_CONFIG_H=1 \
	-I$(top_srcdir)/libavutil \
	-I$(top_srcdir)/libavcodec \
	$(GPROF_FLAGS) \
	$(WARNERR_FLAGS) \
	$(ALTIVEC_CFLAGS)

LDADD = \
	$(GPROF_FLAGS) \
	$(WARNERR_FLAGS)

noinst_LTLIBRARIES = \
	libavformat.la

if CONFIG_ENCODERS
encoder_SRC = \
	mpegtsenc.c \
	movenc.c \
	flvenc.c \
	asf-enc.c
endif

if CONFIG_AUDIO_OSS
audiooss_SRC = audio.c
endif

if CONFIG_AUDIO_BEOS
audiobeos_SRC = beosaudio.cpp
audiobeos_LIBS = \
	-lbe \
	-lmedia
endif

if CONFIG_VIDEO4LINUX
v4l_SRC = grab.c
endif

if CONFIG_BKTR
bktr_SRC = grab_bktr.c
endif

if CONFIG_DV1394
dv_SRC = dv1394.c
endif

if CONFIG_VORBIS
ogg_SRC = ogg.c
ogg_LIBS = -logg
endif

image_SRC = \
	pnm.c \
	yuv.c \
	png.c \
	jpeg.c \
	gifdec.c

file_SRC = \
	avio.c \
	aviobuf.c \
	file.c

if CONFIG_HAVE_DLOPEN
frame_SRC = \
	framehook.c
endif

if CONFIG_NETWORK
if NEED_INET_ATON
aton_SRC = barpainet.c
endif
network_SRC = \
	$(aton_SRC) \
	udp.c \
	tcp.c \
	http.c \
	rtsp.c \
	rtp.c \
	rtpproto.c
endif

common_SRC = \
	utils.c \
	cutils.c \
	os_support.c \
	allformats.c

format_SRC = \
	mpeg.c \
	mpegts.c \
	ffm.c \
	crc.c \
	img.c \
	img2.c \
	raw.c \
	rm.c \
	avienc.c \
	avidec.c \
	wav.c \
	swf.c \
	au.c \
	gif.c \
	mov.c \
	mpjpeg.c \
	dv.c \
	yuv4mpeg.c \
	4xm.c \
	flvdec.c \
	psxstr.c \
	idroq.c \
	ipmovie.c \
	nut.c \
	wc3movie.c \
	mp3.c \
	westwood.c \
	segafilm.c \
	idcin.c \
	flic.c \
	sierravmd.c \
	matroska.c \
	sgi.c \
	sol.c \
	electronicarts.c \
	nsvdec.c \
	ogg2.c \
	oggparsevorbis.c \
	oggparseflac.c \
	oggparsetheora.c \
	mmf.c \
	asf.c \
	daud.c

libavformat_la_SOURCES = \
	$(common_SRC) \
	$(format_SRC) \
	$(encoder_SRC) \
	$(image_SRC) \
	$(audiooss_SRC) \
	$(audiobeos_SRC) \
	$(v4l_SRC) \
	$(bktr_SRC) \
	$(dv_SRC) \
	$(ogg_SRC) \
	$(file_SRC) \
	$(network_SRC) \
	$(frame_SRC)

libavformat_la_LIBADD = \
	$(top_builddir)/libavcodec/libavcodec.la

libavformat_la_LDFLAGS = \
	$(M_LIBS) \
	$(Z_LIBS) \
	$(DL_LIBS) \
	$(audiobeos_LIBS) \
	$(ogg_LIBS)

libavformatdir = $(includedir)/ffmpeg
libavformat_HEADERS = \
	avformat.h \
	avio.h \
	rtp.h \
	rtsp.h \
	rtspcodes.h

noinst_HEADERS = \
	avi.h \
	asf.h \
	barpainet.h \
	dv1394.h \
	dv.h \
	framehook.h \
	mpegts.h \
	os_support.h \
	qtpalette.h \
	ogg2.h
