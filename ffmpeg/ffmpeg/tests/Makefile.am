#if HAVE_MMX
#dsptest_progs = dsptest
#endif

noinst_PROGRAMS = \
	audiogen \
	rotozoom \
	tiny_psnr \
	videogen

#dsptest_CFLAGS = \
#	-DHAVE_AV_CONFIG_H=1 \
#	-I$(top_srcdir)/libavcodec \
#	-I$(top_srcdir)/libavcodec/i386
#dsptest_LDFLAGS = \
#	$(M_LIBS)

vsynth1/0.pgm: videogen
	@mkdir -p vsynth1
	$(srcdir)/videogen 'vsynth1/'

vsynth2/0.pgm: rotozoom
	@mkdir -p vsynth2
	$(srcdir)/rotozoom 'vsynth2/' $(srcdir)/lena.pnm

asynth1.sw: audiogen
	$(srcdir)/audiogen $@

SERVER_REFFILE = ffserver.regression.ref
SERVER_CONFFILE = test.conf

servertest: vsynth1/0.pgm asynth1.sw
	@$(srcdir)/server-regression.sh $(srcdir)/$(SERVER_REFFILE) \
		$(srcdir)/$(SERVER_CONFFILE)

LIBAV_REFFILE = libav.regression.ref

libavtest: vsynth1/0.pgm asynth1.sw
	@$(srcdir)/regression.sh $@ $(srcdir)/$(LIBAV_REFFILE) vsynth1

CODEC_REFFILE1 = ffmpeg.regression.ref
CODEC_REFFILE2 = rotozoom.regression.ref

codectest mpeg4 mpeg ac3: vsynth1/0.pgm vsynth2/0.pgm asynth1.sw tiny_psnr
	@$(srcdir)/regression.sh $@ $(srcdir)/$(CODEC_REFFILE1) vsynth1
	@$(srcdir)/regression.sh $@ $(srcdir)/$(CODEC_REFFILE2) vsynth2

test: codectest libavtest servertest
	true

EXTRA_DIST = \
	regression.sh \
	$(CODEC_REFFILE1) \
	$(CODEC_REFFILE2) \
	$(LIBAV_REFFILE) \
	server-regression.sh \
	$(SERVER_CONFFILE) \
	$(SERVER_REFFILE)
