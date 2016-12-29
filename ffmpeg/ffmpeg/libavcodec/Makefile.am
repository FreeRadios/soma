DIST_SUBDIRS = \
	alpha \
	armv4l \
	i386 \
	liba52 \
	libpostproc \
	mlib \
	ppc \
	ps2 \
	sh4 \
	sparc

if ARCH_ALPHA
alpha_libs = alpha/libalpha.la
alpha_dirs = alpha
endif

if ARCH_ARMV4L
if HAVE_IWMMXT
iwmmxt_libs = armv4l/libiwmmxt.la
endif
armv4l_libs = armv4l/libarmv4l.la $(iwmmxt_libs)
armv4l_dirs = armv4l
endif

if HAVE_MMX
mmxsse_libs = i386/libmmxsse.la
mmxsse_dirs = i386
endif

if HAVE_MLIB
mlib_libs = mlib/libmlib.la
mlib_dirs = mlib
endif

if ARCH_POWERPC
ppc_libs = ppc/libppc.la
if HAVE_ALTIVEC
altivec_libs = ppc/libaltivec.la
endif
ppc_dirs = ppc
endif

if HAVE_MMI
mmi_libs = ps2/libmmi.la
mmi_dirs = ps2
endif

if ARCH_SH4
sh4_libs = sh4/libsh4.la
sh4_dirs = sh4
endif

if ARCH_SPARC
sparc_libs = sparc/libsparc.la
sparc_dirs = sparc
endif

if CONFIG_PP
if SHARED_PP
pp_libs = libpostproc/libpostproc.la
else
pp_libs = libpostproc/libffpostproc.la
endif
pp_dirs = libpostproc
endif

SUBDIRS = \
	$(alpha_dirs) \
	$(armv4l_dirs) \
	$(mmxsse_dirs) \
	$(mlib_dirs) \
	$(mmi_dirs) \
	$(ppc_dirs) \
	$(sh4_dirs) \
	$(sparc_dirs) \
	$(pp_dirs)

cpu_libs = \
	$(alpha_libs) \
	$(armv4l_libs) \
	$(mmxsse_libs) \
	$(mlib_libs) \
	$(mmi_libs) \
	$(ppc_libs) \
	$(altivec_libs) \
	$(sh4_libs) \
	$(sparc_libs)

INCLUDES = \
	-I$(top_srcdir)/libavutil \
	-DHAVE_AV_CONFIG_H=1 \
	$(GPROF_FLAGS) \
	$(WARNERR_FLAGS) \
	$(ALTIVEC_CFLAGS)

LDADD = \
	$(GPROF_FLAGS) \
	$(WARNERR_FLAGS)

noinst_LTLIBRARIES = \
	libavcodec.la

if CONFIG_MP3LAME
lame_SRC = mp3lameaudio.c
lame_LIBS = -lmp3lame
endif

if CONFIG_VORBIS
vorbis_SRC = oggvorbis.c
vorbis_LIBS = \
	-lvorbis \
	-lvorbisenc
endif

if CONFIG_FAAD
faad_SRC = faad.c
if CONFIG_FAADBIN
faad_LIBS = $(DL_LIBS)
else
faad_LIBS = -lfaad
endif
endif

if CONFIG_FAAC
faac_SRC = faac.c
faac_LIBS = -lfaac
endif

if CONFIG_AC3
ac3_SRC = a52dec.c
if CONFIG_A52BIN
ac3_LIBS = $(DL_LIBS)
else
ac3_LIBS = liba52/liba52.la
endif
endif

common_SRC = \
	bitstream.c \
	utils.c \
	opt.c \
	mem.c \
	allcodecs.c \
	mpegvideo.c \
	jrevdct.c \
	jfdctfst.c \
	jfdctint.c \
	mpegaudio.c \
	ac3enc.c \
	mjpeg.c \
	resample.c \
	dsputil.c \
	motion_est.c \
	imgconvert.c \
	imgresample.c \
	mpeg12.c \
	mpegaudiodec.c \
	pcm.c \
	simple_idct.c \
	ratecontrol.c \
	adpcm.c \
	dv.c \
	error_resilience.c \
	fft.c \
	mdct.c \
	mace.c \
	huffyuv.c \
	cyuv.c \
	raw.c \
	h261.c \
	h264.c \
	h264idct.c \
	golomb.c \
	vp3.c \
	vp3dsp.c \
	asv1.c \
	4xm.c \
	cabac.c \
	ffv1.c \
	ra144.c \
	ra288.c \
	vcr1.c \
	cljr.c \
	roqvideo.c \
	dpcm.c \
	interplayvideo.c \
	xan.c \
	rpza.c \
	cinepak.c \
	msrle.c \
	msvideo1.c \
	vqavideo.c \
	idcinvideo.c \
	adx.c \
	faandct.c \
	8bps.c \
	smc.c \
	parser.c \
	flicvideo.c \
	truemotion1.c \
	vmdav.c \
	lcl.c \
	flac.c \
	g726.c \
	png.c \
	pnm.c \
	qpeg.c \
	qtrle.c \
	rangecoder.c \
	resample2.c \
	sonic.c \
	snow.c \
	tscc.c \
	ulti.c \
	qdrw.c \
	xl.c \
	vc9.c \
	loco.c \
	alac.c \
	shorten.c \
	wnv1.c \
	aasc.c \
	indeo2.c \
	ws-snd1.c \
	dvdsub.c \
	dvbsub.c \
	dvbsubdec.c \
	fraps.c \
	vorbis.c \
	h263.c \
	msmpeg4.c \
	h263dec.c \
	svq1.c \
	rv10.c \
	wmadec.c \
	indeo3.c \
	truemotion2.c \
	qdm2.c

libavcodec_la_SOURCES = \
	$(common_SRC) \
	$(lame_SRC) \
	$(vorbis_SRC) \
	$(faad_SRC) \
	$(faac_SRC) \
	$(ac3_SRC)

libavcodec_la_LIBADD = \
	$(cpu_libs) \
	$(pp_libs) \
	$(top_builddir)/libavutil/libavutil.la

libavcodec_la_LDFLAGS = \
	$(M_LIBS) \
	$(Z_LIBS) \
	$(lame_LIBS) \
	$(vorbis_LIBS) \
	$(faad_LIBS) \
	$(faac_LIBS) \
	$(ac3_LIBS)

tests_pass = \
	imgresample-test \
	dct-test \
	motion-test \
	fft-test

noinst_PROGRAMS = \
	apiexample \
	$(tests_pass)

apiexample_SOURCES = apiexample.c
apiexample_LDADD = libavcodec.la

imgresample_test_SOURCES = imgresample.c
imgresample_test_CFLAGS = -DTEST
imgresample_test_LDADD = \
	$(top_builddir)/libavutil/libavutil.la \
	libavcodec.la
imgresample_test_LDFLAGS = \
	$(M_LIBS)

dct_test_SOURCES = \
	dct-test.c \
	fdctref.c
dct_test_LDADD = \
	$(top_builddir)/libavutil/libavutil.la \
	libavcodec.la
dct_test_LDFLAGS = \
	$(M_LIBS)

motion_test_SOURCES = motion_test.c
motion_test_LDADD = \
	$(top_builddir)/libavutil/libavutil.la \
	libavcodec.la
motion_test_LDFLAGS = \
	$(M_LIBS)

fft_test_SOURCES = fft-test.c
fft_test_LDADD = \
	$(top_builddir)/libavutil/libavutil.la \
	libavcodec.la
fft_test_LDFLAGS = \
	$(M_LIBS)

libavcodecdir = $(includedir)/ffmpeg
libavcodec_HEADERS = \
	avcodec.h \
	bitstream.h \
	opt.h

inc_SOURCES = \
	mdec.c \
	motion_est_template.c \
	svq3.c \
	wmv2.c

noinst_HEADERS = \
	ac3.h \
	ac3tab.h \
	cabac.h \
	dsputil.h \
	dvdata.h \
	faandct.h \
	golomb.h \
	h261data.h \
	h263data.h \
	h264data.h \
	imgconvert_template.h \
	indeo3data.h \
	indeo2data.h \
	mpeg12data.h \
	mpeg4data.h \
	mpegaudiodectab.h \
	mpegaudio.h \
	mpegaudiotab.h \
	mpegvideo.h \
	msmpeg4data.h \
	qdm2data.h \
	ra144.h \
	ra288.h \
	rangecoder.h \
	simple_idct.h \
	sp5x.h \
	svq1_cb.h \
	svq1_vlc.h \
	truemotion1data.h \
	ulti_cb.h \
	vp3data.h \
	wmadata.h \
	vc9data.h \
	vorbis.h \
	$(inc_SOURCES)
