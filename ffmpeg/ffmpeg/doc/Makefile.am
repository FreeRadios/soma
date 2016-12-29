htmldir = $(pkgdatadir)/html
if HAVE_TEXI2HTML
html_pages = \
	ffmpeg-doc.html \
	faq.html \
	ffserver-doc.html \
	ffplay-doc.html \
	hooks.html
endif
html_DATA = $(html_pages)

if HAVE_POD2MAN
man_pages = \
	ffmpeg.1 \
	ffserver.1 \
	ffplay.1
endif
man_MANS = $(man_pages)

CLEANFILES = \
	$(man_MANS) \
	$(html_DATA) \
	$(man_MANS:.1=.pod)

EXTRA_DIST = \
	ffmpeg_powerpc_performance_evaluation_howto.txt \
	ffserver.conf \
	optimization.txt \
	texi2pod.pl \
	TODO \
	$(html_DATA:.html=.texi) \
	$(man_MANS:.1=-doc.texi)

%.html: %.texi
	$(TEXI2HTML) -monolithic -number $<

%.pod: %-doc.texi
	$(srcdir)/texi2pod.pl $< $@

%.1: %.pod
	$(POD2MAN) --section=1 --center=" " --release=" " $< > $@
