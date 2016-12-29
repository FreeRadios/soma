INCLUDES = \
	-I$(top_srcdir)/libavutil \
	-I$(top_srcdir)/libavcodec \
	-DHAVE_AV_CONFIG_H=1

noinst_LTLIBRARIES = libalpha.la

libalpha_la_SOURCES = \
	dsputil_alpha.c \
	mpegvideo_alpha.c \
	simple_idct_alpha.c \
	motion_est_alpha.c \
	dsputil_alpha_asm.S \
	motion_est_mvi_asm.S

libalpha_la_CFLAGS = \
	-fforce-addr \
	-freduce-all-givs

noinst_HEADERS = \
	regdef.h \
	asm.h
