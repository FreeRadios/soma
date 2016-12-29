if ENABLE_SM
sm = sm
endif

if !ENABLE_FFMPEG
ffmpeg = ffmpeg
endif

if ENABLE_CONFUSE
confuse = confuse
endif

SUBDIRS = \
	ghthash \
	$(ffmpeg) \
	$(confuse) \
	$(sm) \
	daemon \
	lib \
	client \
	check \
	include \
	config \
	pysoma \
	phpsoma

DIST_SUBDIRS = \
	confuse \
	ghthash \
	ffmpeg \
	sm \
	daemon \
	lib \
	client \
	check \
	include \
	config \
	pysoma \
	phpsoma

EXTRA_DIST = \
	README.module \
	soma.cfg_example \
	palinsesto.cfg_example \
	spot.cfg_example \
	palinsesto.cfg.5 \
	spot.cfg.5 \
	soma.cfg.5 \
	somacheck.1 \
	somaclient.1 \
	somad.1 \
	soma-config.1 \
	somad_crash.py \
	soma-xml-palinsesto.dtd \
	soma-xml-spot.dtd

cfg_DATA = \
	soma.cfg_example \
	palinsesto.cfg_example \
	spot.cfg_example

cfgdir = $(sysconfdir)/somad

man_MANS = \
	palinsesto.cfg.5 \
	spot.cfg.5 \
	soma.cfg.5 \
	somacheck.1 \
	somaclient.1 \
	somad.1 \
	soma-config.1
