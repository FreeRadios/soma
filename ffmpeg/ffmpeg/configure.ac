dnl Replaces old FFMpeg build system
dnl Note: PPC, Darwin, CPU tuning and mingw32 has been removed. That
dnl does not belong in a configure.ac file. The BeOS stuff seems a
dnl bit weird to me too (as if it's system-specific), so removed
dnl until I know what it does (that's the netserver stuff).
dnl Lastly, amr_[nw]b removed because it's not autotoolized. Someone
dnl please re-add that sanely. Can do it myself if I know how it works.
dnl fastmemcpy can be done using make CFLAGS="-include fastmemcpy.h"

dnl TODO: implement "make test"
dnl TODO: fix all .c files to conditionally include config.h
dnl TODO: and .h files shouldn't include config.h at all
dnl         ./libavcodec/common.h
dnl         ./libavformat/barpainet.h
dnl TODO: remove HAVE_AV_CONFIG_H everywhere
dnl TODO: split public/private parts from avformat.h/avcodec.h

AC_INIT(FFMpeg, 0.4.8)
AC_CANONICAL_TARGET([])
AC_CONFIG_AUX_DIR([.])

AC_CONFIG_SRCDIR(ffmpeg.c)
AM_INIT_AUTOMAKE(ffmpeg, ${PACKAGE_VERSION})
PACKAGE_TARNAME=$PACKAGE

AM_CONFIG_HEADER(config.h)

AM_MAINTAINER_MODE
AC_GNU_SOURCE
AM_PROG_AS
AC_PROG_CC
AC_PROG_INSTALL
AC_SYS_LARGEFILE
AC_STDC_HEADERS
AM_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl Warning for anyone using these custom macros.
dnl before asking me questions on why it doesn't work and why you
dnl are getting warnings about uninitialized variables being used,
dnl make sure that any call to AC_FF_ALLOW_DISABLE/ENABLE is the
dnl outermost macro in your series of macros. If you don't do this,
dnl e.g. by embedding it inside an if cpu=x86 check, then this piece
dnl of macro will not be evaluated if not on x86. The resulting
dnl variable will be used in AC_DEFINE, which causes the error.
dnl Always make these your outermost macro.

dnl Add enable/disable options with conditional/define
dnl Arts: full variable name, description
AC_DEFUN([AC_FF_DEFINE],[
AM_CONDITIONAL($1, test "x$$1" = "xyes")
if test "x$$1" = "xyes"; then
  AC_DEFINE([$1], 1, [$2])
fi
])
dnl Args: variable name, short name, description, action-if-given,
dnl       action-if-not-given, default, other, word, cword
AC_DEFUN([AC_FF_ALLOW_OPT],[
AC_ARG_ENABLE([$2],[AC_HELP_STRING([--$8-$2],[$9 $3])],[
              ifelse([$4],,[$1=$7],[$4])],[
              ifelse([$5],,[$1=$6],[$5])])
AC_FF_DEFINE($1,[$3])
])
dnl Args: variable name, short name, description, [action-if, [action=if-not]]
AC_DEFUN([AC_FF_ALLOW_ENABLE],[
AC_FF_ALLOW_OPT([$1],[$2],[$3],[$4],[$5],no,yes,enable,Enable)
])
dnl Args: variable name, short name, description, [action-if, [action=if-not]]
AC_DEFUN([AC_FF_ALLOW_DISABLE],[
AC_FF_ALLOW_OPT([$1],[$2],[$3],[$5],[$4],yes,no,disable,Disable)
])
dnl Encoders/Decoders - all enabled by default
AC_DEFUN([AC_FF_ENCODER],[
AC_FF_ALLOW_DISABLE([CONFIG_$1_ENCODER],[encoder-$2],[$3 encoder],[
	if test x$CONFIG_ENCODERS = xyes; then
		CONFIG_$1_ENCODER=yes
	else
		CONFIG_$1_ENCODER=no
	fi
])])
AC_DEFUN([AC_FF_DECODER],[
AC_FF_ALLOW_DISABLE([CONFIG_$1_DECODER],[decoder-$2],[$3 decoder],[
	if test x$CONFIG_DECODERS = xyes; then
		CONFIG_$1_DECODER=yes
	else
		CONFIG_$1_DECODER=no
	fi
])])

dnl Default compiler stuff
OLD_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall -Wno-switch"
AC_MSG_CHECKING(whether compiler understands -Wall/-Wno-switch)
AC_TRY_COMPILE([],[ return 0; ],[
  WARNERR_FLAGS="$WARNERR_FLAGS -Wall -Wno-switch" && AC_MSG_RESULT(yes) ],[
  WARNERR_FLAGS="$WARNERR_FLAGS" && AC_MSG_RESULT(no) ])
CFLAGS="$OLD_CFLAGS"
AC_SUBST(WARNERR_FLAGS)

dnl Enable cross-compiling
if test x$host_cpu != x; then
  TARGET_CPU="$host_cpu"
else
  TARGET_CPU=`uname -p`
fi

if test x$host_os != x; then
  TARGET_OS="$host_os"
else
  TARGET_OS=`uname -s`
fi

CONFIG_OS2=no
CONFIG_WIN32=no
CONFIG_SUNOS=no
CONFIG_DARWIN=no

case "$TARGET_OS" in
  linux*)
    TARGET_OS=linux
    ;;
  freebsd|netbsd|bsdos|openbsd|qnx|gnu|beos)
    ;;
  sunos)
    CONFIG_SUNOS=yes
    ;;
  darwin*)
    CONFIG_DARWIN=yes
    ;;
  os2)
    CONFIG_OS2=yes
    ;;
  cygwin*)
    CONFIG_WIN32=yes
    ;;
  irix*)
    TARGET_OS="irix"
    ;;
  hp*ux*)
    TARGET_OS="hpux"
    ;;
  *)
    TARGET_OS="$TARGET_OS-unknown"
    ;;
esac

AC_FF_DEFINE(CONFIG_OS2,    [Building for OS/2])
AC_FF_DEFINE(CONFIG_WIN32,  [Building for Win32 (cygwin/mingw32)])
AC_FF_DEFINE(CONFIG_SUNOS,  [Building for SunOS])
AC_FF_DEFINE(CONFIG_DARWIN, [Building for Darwin])

if test "$CONFIG_DARWIN" = "yes"; then
  if test -n "`$CC -v 2>&1 | grep xlc`"; then
    CFLAGS="$CFLAGS -qpdf2 -qlanglvl=extc99 -qmaxmem=-1 -qarch=auto -qtune=auto"
  else
    gcc_version="`$CC -v 2>&1 | grep version | cut -d ' ' -f3-`"
    case "$gcc_version" in
      *2.95*)
        CFLAGS="$CFLAGS -no-cpp-precomp -pipe -fomit-frame-pointer"
      ;;
      *[34].*)
        CFLAGS="$CFLAGS -no-cpp-precomp -pipe -fomit-frame-pointer -force_cpusubtype_ALL -Wno-sign-compare"
      ;;
      *)
        CFLAGS="$CFLAGS -no-cpp-precomp -pipe -fomit-frame-pointer"
      ;;
    esac
  fi
fi

ARCH_ARMV4L=no
ARCH_ALPHA=no
ARCH_X86=no
ARCH_MIPS=no
ARCH_POWERPC=no
ARCH_SH4=no
ARCH_SPARC64=no
ARCH_SPARC=no

dnl Define arch (copied from old Makefile, seems like a reasonable table)
case "$TARGET_CPU" in
  i386|i486|i586|i686|i86pc|bepc)
    TARGET_ARCH="x86"
    ARCH_X86=yes
    ;;
  # armv4l is a subset of armv5tel
  armv4l|armv5tel)
    TARGET_ARCH="armv4l"
    ARCH_ARMV4L=yes
    ;;
  alpha)
    TARGET_ARCH="alpha"
    ARCH_ALPHA=yes
    ;;
  powerpc|ppc)
    TARGET_ARCH="powerpc"
    ARCH_POWERPC=yes
    ;;
  mips)
    TARGET_ARCH="mips"
    ARCH_MIPS=yes
    ;;
  sun4u|sparc64)
    TARGET_ARCH="sparc64"
    ARCH_SPARC64=yes
    ARCH_SPARC=yes
    ;;
  sh4)
    TARGET_ARCH="sh4"
    ARCH_SH4=yes
    ;;
  sparc)
    TARGET_ARCH="sparc"
    ARCH_SPARC=yes
    ;;
  *)
    TARGET_ARCH="unknown"
    ;;
esac

AC_FF_DEFINE(ARCH_ARMV4L,  [building for ARM platform])
AC_FF_DEFINE(ARCH_ALPHA,   [building for Alpha platform])
AC_FF_DEFINE(ARCH_X86,     [building for x86 platform])
AC_FF_DEFINE(ARCH_MIPS,    [building for MIPS platform])
AC_FF_DEFINE(ARCH_POWERPC, [building for PowerPC platform])
AC_FF_DEFINE(ARCH_SH4,     [building for Sh4 platform])
AC_FF_DEFINE(ARCH_SPARC64, [building for Sparc64 platform])
AC_FF_DEFINE(ARCH_SPARC,   [building for Sparc platform])

dnl First, check endianness - mind cross-compiling
if test x$(uname -p) = x$TARGET_CPU; then
  AC_C_BIGENDIAN(WORDS_BIGENDIAN=yes, WORDS_BIGENDIAN=no)
else
  if test x$TARGET_ARCH = xalpha -o x$TARGET_CPU = xpowerpc; then
    WORDS_BIGENDIAN=yes
  else
    WORDS_BIGENDIAN=no
  fi
fi
AC_FF_DEFINE(WORDS_BIGENDIAN, processor is big-endian)

dnl Math lib is pretty much required
AC_CHECK_HEADER(math.h,,AC_MSG_ERROR(math.h not found))
FOUND_M_LIB=no
AC_CHECK_FUNC(log,[ M_LIBS="" && FOUND_M_LIB=yes ],[
  dnl prefer libcpml (compaq math lib) on alpha
  if test x$TARGET_OS = xalpha; then
    AC_CHECK_LIB(cpml, log,[ FOUND_M_LIB=yes && M_LIBS="-lcpml" ])
  fi
  if test x$FOUND_M_LIB = xno; then
    AC_CHECK_LIB(m, log, M_LIBS="-lm",[
                 AC_MSG_ERROR(No math libs found)])
  fi
])
AC_SUBST(M_LIBS)

dnl Check for inttypes.h
AC_CHECK_HEADER(inttypes.h,,[
  AC_DEFINE(EMULATE_INTTYPES, 1, [ Emulate inttypes.h if not available ])
])

dnl Fast integer types
AC_MSG_CHECKING(for fast integer types)
AC_TRY_COMPILE([
#include <inttypes.h>
  ],[
    volatile uint_fast64_t i=0x01234567;
    return 0;
  ],[ AC_MSG_RESULT(yes) ],[ AC_MSG_RESULT(no) &&
    AC_DEFINE(EMULATE_FAST_INT, 1, [ Emulate fast integer types ])
])

dnl MMX
AC_FF_ALLOW_DISABLE(HAVE_MMX, mmx, use MMX optimizations,[
  if test x$TARGET_ARCH = xx86; then
    HAVE_MMX=yes
  else
    HAVE_MMX=no
  fi
])

dnl vector types - only used in x86 optimizations right now
if test x$TARGET_ARCH = xx86; then
  AC_MSG_CHECKING(for vector types)
  AC_TRY_COMPILE([
#if __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ < 2)
#error no vectors built-in
#endif
    ],[ return 0; ],[ HAVE_BUILTIN_VECTOR=yes && AC_MSG_RESULT(yes) ],[
      HAVE_BUILTIN_VECTOR=no && AC_MSG_RESULT(no) ])
else
  HAVE_BUILTIN_VECTOR=no
fi
AC_FF_DEFINE(HAVE_BUILTIN_VECTOR,[Have built-in vector types])

dnl MMI
if test x$TARGET_ARCH = xmips; then
  AC_MSG_CHECKING(for MMI instruction set)
  AC_TRY_COMPILE([],[
    __asm__ ("lq \$2, 0(\$2)");
    return 0;
  ],[ HAVE_MMI=yes && AC_MSG_RESULT(yes) ],[
    HAVE_MMI=no && AC_MSG_RESULT(no) ])
else
  HAVE_MMI=no
fi
AC_FF_DEFINE(HAVE_MMI,[use MMI optimizations])

dnl Altivec
AC_FF_ALLOW_DISABLE(HAVE_ALTIVEC, altivec, use Altivec optimizations,[
  if test x$TARGET_ARCH = xpowerpc; then
    # AltiVec flags: The FSF version of GCC differs from the Apple version
    if test -n "`$CC -v 2>&1 | grep version | grep Apple`"; then
      ALTIVEC_CFLAGS="-faltivec"
    else
      ALTIVEC_CFLAGS="-maltivec -mabi=altivec"
    fi
    OLD_CFLAGS="$CFLAGS"
    OLD_CPPFLAGS="$CPPFLAGS"
    CFLAGS="$CFLAGS $ALTIVEC_CFLAGS"
    CPPFLAGS="$CPPFLAGS $ALTIVEC_CFLAGS"
    AC_CHECK_HEADER(altivec.h, [ ALT_H="#include <altivec.h>"
                    HAVE_ALTIVEC_H=yes ],  ALT_H="" HAVE_ALTIVEC_H=no)
    # See does our compiler support Motorola AltiVec C API
    AC_MSG_CHECKING(for support of Motorola Altivec API)
    AC_TRY_COMPILE($ALT_H,[
        vector signed int v1, v2, v3;
        v1 = vec_add(v2,v3);
        return 0;
      ],[ HAVE_ALTIVEC=yes && AC_MSG_RESULT(yes) ],[
        HAVE_ALTIVEC=no && AC_MSG_RESULT(no) ])
    if test x$HAVE_ALTIVEC = xyes; then
      if test x$HAVE_ALTIVEC_H = xyes; then
        AC_DEFINE(HAVE_ALTIVEC_H, 1,[altivec.h available])
      fi
      AC_SUBST(ALTIVEC_CFLAGS)
    fi
    CFLAGS="$OLD_CFLAGS"
    CPPFLAGS="$OLD_CPPFLAGS"
  else
    HAVE_ALTIVEC=no
  fi
])

dnl IWMMXT
AC_FF_ALLOW_DISABLE(HAVE_IWMMXT, iwmmxt, use ARM/IWMMXT optimizations,[
  if test x$TARGET_ARCH = xarmv4l; then
    AC_MSG_CHECKING(for support of IWMMXT optimizations)
    AC_TRY_COMPILE(,[
        __asm__ __volatile__ ("wunpckelub wr6, wr4");
        return 0;
      ],[ HAVE_IWMMXT=yes && AC_MSG_RESULT(yes) ],[
        HAVE_IWMMXT=no && AC_MSG_RESULT(no) ])
  else
    HAVE_IWMMXT=no
  fi
])

dnl GProf (-p)
AC_FF_ALLOW_ENABLE(HAVE_GPROF, gprof, profiling with gprof,[
  GPROF_FLAGS="-p"
  AC_SUBST(GPROF_FLAGS)
  HAVE_GPROF=yes
])

dnl PowerPC performance reporting
AC_FF_ALLOW_ENABLE(POWERPC_PERFORMANCE_REPORT, ppc-performance,
  [enable powerpc performance reporting],[
  if test x$TARGET_ARCH = xpowerpc; then
    POWERPC_PERFORMANCE_REPORT=yes
  else
    POWERPC_PERFORMANCE_REPORT=no
  fi
])

dnl localtime_r() check
AC_CHECK_FUNC(localtime_r,[
  AC_DEFINE(HAVE_LOCALTIME_R, 1, [whether localtime_r() is available])
])

dnl lrintf()
OLD_LIBS="$LIBS"
LIBS="$LIBS $M_LIBS"
AC_CHECK_FUNC(lrintf,[
  AC_DEFINE(HAVE_LRINTF, 1, [whether lrintf() is available])
])
LIBS="$OLD_LIBS"

dnl Check for malloc.h
AC_CHECK_HEADER(malloc.h,[
  AC_DEFINE(HAVE_MALLOC_H, 1, [whether malloc.h available])
])

dnl Check for memalign
AC_CHECK_FUNC(posix_memalign,[
  AC_DEFINE(HAVE_POSIX_MEMALIGN, 1, [whether memalign() is available])
])

dnl libz probing
Z_LIBS=""
AC_FF_ALLOW_DISABLE(CONFIG_ZLIB, zlib, use zlib for decompression,[
  AC_CHECK_HEADER(zlib.h,[
    AC_CHECK_FUNC(uncompress,[ CONFIG_ZLIB=yes && Z_LIBS="" ],[
      AC_CHECK_LIB(z, uncompress,[ CONFIG_ZLIB=yes && Z_LIBS="-lz" ],[
                   CONFIG_ZLIB=no ])
    ])
  ], CONFIG_ZLIB=no)
])
AC_SUBST(Z_LIBS)

dnl libdl/dlopen/dlfcn probinga
AC_CHECK_HEADER(dlfcn.h,[ CONFIG_HAVE_DLFCN=yes
  AC_DEFINE(CONFIG_HAVE_DLFCN, 1, [whether dlfcn.h is available]) ], [
  CONFIG_HAVE_DLFCN=no])
DL_LIBS=""
AC_CHECK_FUNC(dlopen,[ CONFIG_HAVE_DLOPEN=yes && DL_LIBS="" ],[
  AC_CHECK_LIB(dl, dlopen,[ CONFIG_HAVE_DLOPEN=yes && DL_LIBS="-ldl" ],[
               CONFIG_HAVE_DLOPEN=no])
])
AC_SUBST(DL_LIBS)
AC_FF_DEFINE(CONFIG_HAVE_DLOPEN, [whether dlopen is available])

dnl restrict keyword finding
restrict=""
for restrict_keyword in restrict __restrict__ __restrict; do
  AC_MSG_CHECKING(for restrict keyword $restrict_keyword)
  AC_TRY_COMPILE([],[ void foo(char * $restrict_keyword p); ],[
                 KEYWORD_FOUND=yes && AC_MSG_RESULT(yes) ],[
                 KEYWORD_FOUND=no && AC_MSG_RESULT(no) ])
  if test x$KEYWORD_FOUND = xyes; then
    restrict="$restrict_keyword"
    break
  fi
done
if test x$restrict = x; then
  AC_MSG_ERROR(No restrict keyword found)
fi
AC_DEFINE_UNQUOTED(restrict, $restrict, [restrict keyword])

dnl GNU getoptlong
GETOPTLONG_LIBS=""
AC_CHECK_FUNC(getopt,,[
  AC_CHECK_LIB(gnugetopt, getopt, GETOPTLONG_LIBS="-lgnugetopt",[
    AC_MSG_ERROR(We require getopt)
  ])
])
AC_SUBST(GETOPTLONG_LIBS)

dnl networking
NETWORK_LIBS=""
AC_FF_ALLOW_DISABLE(CONFIG_NETWORK, network, rtp/http networking code,[
  CONFIG_NETWORK=yes
  AC_CHECK_FUNC(inet_aton, NEED_INET_ATON=no, NEED_INET_ATON=yes)
  AC_CHECK_FUNC(connect,[ NETWORK_LIBS="" ],[
    AC_CHECK_LIB(nsl, connect,[ NETWORK_LIBS="-lnsl" ],[
                 CONFIG_NETWORK=no ])
  ])
], CONFIG_NETWORK=no && NEED_INET_ATON=no)
AM_CONDITIONAL(NEED_INET_ATON, test x$NEED_INET_ATON = xyes)
AC_SUBST(NETWORK_LIBS)

dnl Here are options to opt in/out parts of the code (default=enable)
AC_FF_ALLOW_DISABLE(CONFIG_DECODERS, decoders, compilation of decoders)
CONFIG_DEMUXERS=$CONFIG_DECODERS
AC_FF_DEFINE(CONFIG_DEMUXERS, compilation of demuxers)
AC_FF_DECODER(H263, h263, h.263)
AC_FF_DECODER(H261, h261, h.261)
AC_FF_DECODER(MPEG4, mpeg4, MPEG-4 video)
AC_FF_DECODER(MSMPEG4V1, msmpeg4v1, Microsoft MPEG-4 v1)
AC_FF_DECODER(MSMPEG4V2, msmpeg4v2, Microsoft MPEG-4 v2)
AC_FF_DECODER(MSMPEG4V3, msmpeg4v3, Microsoft MPEG-4 v3)
AC_FF_DECODER(WMV1, wmv1, Windows Media Video v1)
AC_FF_DECODER(WMV2, wmv2, Windows Media Video v2)
AC_FF_DECODER(WMV3, wmv3, Windows Media Video v3)
AC_FF_DECODER(VC9, vc9, Microsoft Video v9)
AC_FF_DECODER(H263I, h263i, Intel h.263)
AC_FF_DECODER(FLV, flv, FLV Video)
AC_FF_DECODER(RV10, rv10, RealVideo 1.0)
AC_FF_DECODER(RV20, rv20, RealVideo 2.0)
AC_FF_DECODER(SVQ1, svq1, Sorensen Video v1)
AC_FF_DECODER(SVQ3, svq3, Sorensen Video v3)
AC_FF_DECODER(WMAV1, wmav1, Windows Media Audio v1)
AC_FF_DECODER(WMAV2, wmav2, Windows Media Audio v2)
AC_FF_DECODER(INDEO2, indeo2, Intel Indeo v2)
AC_FF_DECODER(INDEO3, indeo3, Intel Indeo v3)
AC_FF_DECODER(TSCC, tscc, TechSmith Camcasia)
AC_FF_DECODER(ULTI, ulti, UltiMotion Video)
AC_FF_DECODER(QDRAW, qdraw, QuickDraw)
AC_FF_DECODER(XL, xl, Miro Video XL)
AC_FF_DECODER(LOCO, loco, LOCO)
AC_FF_DECODER(WNV1, wnv1, Winnov v1)
AC_FF_DECODER(AASC, aasc, Autodesk RLE)
AC_FF_DECODER(FRAPS, fraps, Lossless Fraps)
dnl faad
AC_FF_DECODER(MPEG1VIDEO, mpeg1video, MPEG-1 video)
AC_FF_DECODER(MPEG2VIDEO, mpeg2video, MPEG-2 video)
AC_FF_DECODER(MPEGVIDEO, mpegvideo, MPEG-1/2 video)
dnl xvmc
AC_FF_DECODER(DVVIDEO, dvvideo, Digital Video)
AC_FF_DECODER(MJPEG, mjpeg, Motion-JPEG)
AC_FF_DECODER(MPEGB, mjpegb, Motion-JPEG B)
AC_FF_DECODER(SP5X, sp5x, SP5X JPEG)
AC_FF_DECODER(PNG, png, Portable Network Graphics)
AC_FF_DECODER(MP2, mp2, MPEG-1 layer 1/2)
AC_FF_DECODER(MP3, mp3, MPEG-1 layer 3)
AC_FF_DECODER(MP3ADU, mp3adu, MPEG-1 layer 3 ADU)
AC_FF_DECODER(MP3ON4, mp3on4, MPEG-1 layer 3 On4)
AC_FF_DECODER(MACE3, mace3, MACE-3)
AC_FF_DECODER(MACE6, mace6, MACE-6)
AC_FF_DECODER(HUFFYUV, huffyuv, Huffyuv Lossless Video)
AC_FF_DECODER(FFVHUFF, ffvhuff, FFMpeg Huffman Video)
AC_FF_DECODER(FFV1, ffv1, FFMpeg Video v1)
AC_FF_DECODER(SNOW, snow, Snow Video)
AC_FF_DECODER(CYUV, cyuv, CYUV)
AC_FF_DECODER(H264, h264, h.264)
AC_FF_DECODER(VP3, vp3, On2 VP-3)
AC_FF_DECODER(THEORA, theora, Theora)
AC_FF_DECODER(ASV1, asv1, Asus Video v1)
AC_FF_DECODER(ASV2, asv2, Asus Video v2)
AC_FF_DECODER(VCR1, vcr1, ATI VCR-1)
AC_FF_DECODER(CLJR, cljr, Cirrus Logic AccuPak)
AC_FF_DECODER(FOURXM, 4xm, 4xm Video)
AC_FF_DECODER(MDEC, mdec, PSX MDEC)
AC_FF_DECODER(ROQ, roq, RoQ Video)
AC_FF_DECODER(INTERPLAY_VIDEO, interplay-video, Interplay Video)
AC_FF_DECODER(XAN_WC3, xan-wc3, Xan WC-3)
AC_FF_DECODER(RPZA, rpza, RPZA Video)
AC_FF_DECODER(CINEPAK, cinepak, Cinepak Video)
AC_FF_DECODER(MSRLE, msrle, Microsoft RLE)
AC_FF_DECODER(MSVIDEO1, msvideo1, Microsoft Video v1)
AC_FF_DECODER(VQA, vqa, Westwood VQA Video)
AC_FF_DECODER(IDCIN, idcin, ID Quake-II CIN Video)
AC_FF_DECODER(EIGHTBPS, 8bps, 8-BPS Video)
AC_FF_DECODER(SMC, smc, Quicktime SMC Graphics)
AC_FF_DECODER(FLIC, flic, FLI/FLV Animation)
AC_FF_DECODER(TRUEMOTION1, truemotion1, TrueMotion-1)
AC_FF_DECODER(TRUEMOTION2, truemotion2, TrueMotion-2)
AC_FF_DECODER(VMDVIDEO, vmdvideo, VMD Video)
AC_FF_DECODER(VMDAUDIO, vmdaudio, VMD Audio)
AC_FF_DECODER(MSZH, mszh, MSZH Lossless Video)
AC_FF_DECODER(ZLIB, zlib, Lossless zlib Compression)
AC_FF_DECODER(SONIC, sonic, Sonic Audio)
dnl ac-3, dts
AC_FF_DECODER(RA_144, ra144, RealAudio 1.0)
AC_FF_DECODER(RA_288, ra288, RealAudio 2.0)
AC_FF_DECODER(ROQ_DPCM, roq-dpcm, RoQ DPCM Audio)
AC_FF_DECODER(INTERPLAY_DPCM, interplay-dpcm, Interplay DPCM)
AC_FF_DECODER(XAN_DPCM, xan-dpcm, Xan DPCM)
AC_FF_DECODER(SOL_DPCM, sol-dpcm, Sol DPCM)
AC_FF_DECODER(QTRLE, qtrle, Quicktime RLE)
AC_FF_DECODER(FLAC, flac, Free Lossless Audio)
AC_FF_DECODER(SHORTEN, shorten, Shorten Lossless Audio)
AC_FF_DECODER(ALAC, alac, Apple Lossless Audio)
AC_FF_DECODER(WS_SND1, ws-snd1, Westwood Sound-1)
AC_FF_DECODER(VORBIS, vorbis, Vorbis Audio)
AC_FF_DECODER(QDM2, qdm2, Q-design Music 2)
dnl amr, gsm

AC_FF_ALLOW_DISABLE(CONFIG_ENCODERS, encoders, compilation of encoders)
CONFIG_MUXERS=$CONFIG_ENCODERS
AC_FF_DEFINE(CONFIG_MUXERS, compilation of muxers)
AC_FF_ENCODER(AC3, ac3, AC-3)
AC_FF_ENCODER(MP2, mp2, MPEG-1 layer 2)
dnl mp3lame, vorbis, faac, xvid
AC_FF_ENCODER(MPEG1VIDEO, mpeg1video, MPEG-1 video)
AC_FF_ENCODER(MPEG2VIDEO, mpeg2video, MPEG-2 video)
AC_FF_ENCODER(H261, h261, h.261)
AC_FF_ENCODER(H263, h263, h.263)
AC_FF_ENCODER(H263P, h263p, h.263+)
AC_FF_ENCODER(FLV, flv, FLV video)
AC_FF_ENCODER(RV10, rv10, RealVideo 1.0)
AC_FF_ENCODER(RV20, rv20, RealVideo 2.0)
AC_FF_ENCODER(MPEG4, mpeg4, MPEG-4 video)
AC_FF_ENCODER(MSMPEG4V1, msmpeg4v1, Microsoft MPEG-4 v1)
AC_FF_ENCODER(MSMPEG4V2, msmpeg4v2, Microsoft MPEG-4 v2)
AC_FF_ENCODER(MSMPEG4V3, msmpeg4v3, Microsoft MPEG-4 v3)
AC_FF_ENCODER(WMV1, wmv1, Windows Media Video v1)
AC_FF_ENCODER(WMV2, wmv2, Windows Media Video v2)
AC_FF_ENCODER(SVQ1, svq1, Sorensen Video v1)
AC_FF_ENCODER(MJPEG, mjpeg, Motion-JPEG)
AC_FF_ENCODER(LJPEG, ljpeg, Lossless JPEG)
AC_FF_ENCODER(PNG, png, Portable Network Graphics)
AC_FF_ENCODER(PPM, ppm, PPM)
AC_FF_ENCODER(PGM, pgm, PGM)
AC_FF_ENCODER(PGMYUV, pgmyuv, PGMYUV)
AC_FF_ENCODER(PBM, pbm, PBM)
AC_FF_ENCODER(PAM, pam, PAM)
AC_FF_ENCODER(HUFFYUV, huffyuv, Huffyuv Lossless Video)
AC_FF_ENCODER(FFVHUFF, ffvhuff, FFMpeg Huffman Video)
AC_FF_ENCODER(ASV1, asv1, Asus Video v1)
AC_FF_ENCODER(ASV2, asv2, Asus Video v2)
AC_FF_ENCODER(FFV1, ffv1, FFMpeg Video v1)
AC_FF_ENCODER(SNOW, snow, Snow)
AC_FF_ENCODER(ZLIB, zlib, Compression Library)
AC_FF_ENCODER(DVVIDEO, dvvideo, Digital Video)
AC_FF_ENCODER(SONIC, sonic, Sonic Audio)
AC_FF_ENCODER(SONIC_LS, sonic-ls, Sonic Lossless Audio)
dnl x264, gsm

dnl video hooks
AC_FF_ALLOW_DISABLE(HAVE_VHOOK, vhook, video hooking plugins,[
  dnl vhook requires dlopen
  if test x$CONFIG_HAVE_DLOPEN != xyes; then
    HAVE_VHOOK=no
  else
    HAVE_VHOOK=yes
  fi
])

dnl simple IDCT routines yes or no
AC_FF_ALLOW_DISABLE(SIMPLE_IDCT, simple-idct, simple IDCT routines)

dnl High-precision (but slower_ MPEG audio functions
AC_FF_ALLOW_ENABLE(CONFIG_MPEGAUDIO_HP, mpegaudio-hp,
                   high-precision (slower) MPEG audio)

dnl libpostproc (GPL, so default=disable)
AC_FF_ALLOW_ENABLE(SHARED_PP, shared-pp, runtime libpostproc.so linking)
AC_FF_ALLOW_ENABLE(CONFIG_PP, pp, post-processing functions)

dnl External lib: lame (default=disable)
AC_FF_ALLOW_ENABLE(CONFIG_MP3LAME, mp3lame, mp3 encoding via libmp3lame,[
  AC_CHECK_HEADER(lame/lame.h,[
    AC_CHECK_LIB(mp3lame, lame_init, CONFIG_MP3LAME=yes,[
                 CONFIG_MP3LAME=no ], $M_LIBS)
  ], CONFIG_MP3LAME=no)
])

dnl External lib: vorbis/ogg (default=disable)
AC_FF_ALLOW_ENABLE(CONFIG_VORBIS, vorbis, vorbis support via libvorbis,[
  XIPH_PATH_VORBIS(CONFIG_VORBIS=yes,CONFIG_VORBIS=no)
])

dnl External lib: FAAD (default=disable)
AC_FF_ALLOW_ENABLE(CONFIG_FAADBIN, faadbin, runtime libfaad.so.0 resolving,[
  dnl Runtime resolving requires libdl
  if test x$CONFIG_HAVE_DLOPEN != xyes; then
    CONFIG_FAADBIN=no
  else
    CONFIG_FAADBIN=yes
  fi
])
AC_FF_ALLOW_ENABLE(CONFIG_FAAD, faad, faad support via libfaad,[
  dnl For both runtime and normal linking, we need the lib
  AC_CHECK_HEADER(faad.h,[
    AC_CHECK_LIB(faad, faacDecOpen, CONFIG_FAAD=yes, CONFIG_FAAD=no, $M_LIBS)
  ], CONFIG_FAAD=no)
])

dnl External lib: FAAC (default=disable)
AC_FF_ALLOW_ENABLE(CONFIG_FAAC, faac, faac support via libfaac,[
  AC_CHECK_HEADER(faac.h,[
    AC_CHECK_LIB(faac, faacEncOpen, CONFIG_FAAC=yes, CONFIG_FAAC=no, $M_LIBS)
  ], CONFIG_FAAC=no)
])

dnl External lib: liba52dec (default=disable)
AC_FF_ALLOW_ENABLE(CONFIG_A52BIN, shared-a52, runtime liba52.so.0 resolving,[
  dnl Runtime linking requires dlopen
  if test x$CONFIG_HAVE_DLOPEN != xyes; then
    CONFIG_A52BIN=no
  else
    CONFIG_A52BIN=yes
  fi
])
AC_FF_ALLOW_ENABLE(CONFIG_AC3, a52, GPL A52 support via liba52,[
  dnl If we're gonna do runtime linking, we'll need the lib
  if test x$CONFIG_A52BIN = xyes; then
    AC_CHECK_HEADER(a52dec/a52.h,[
      AC_CHECK_LIB(a52, a52_init, CONFIG_AC3=yes, CONFIG_AC3=no, $M_LIBS)
    ], CONFIG_AC3=no)
  else
    CONFIG_AC3=yes
  fi
])

dnl External lib: sun media lib (used for optimizations)
AC_FF_ALLOW_ENABLE(HAVE_MLIB, sunmlib, support for Sun Media Lib,[
  AC_CHECK_HEADER(mlib_types.h,[
    AC_CHECK_LIB(mlib, mlib_VectorConvert_S16_U8_Mod, HAVE_MLIB=yes,[
                 HAVE_MLIB=no ], $M_LIBS)
  ], HAVE_MLIB=no)
])

dnl fixed/floating point AMR codecs
AC_FF_ALLOW_ENABLE(AMR_NB, amr_nb, floating-point amr_nb audio codec)
AC_FF_ALLOW_ENABLE(AMR_NB_FIXED, amr_nb-fixed, fixed-point amr_nb audio codec,[
  if test x$AMR_NB = xyes; then
    echo "AMR NB FIXED POINT NOTICE! Make sure you have downloaded TS26.073 "
    echo "REL-5 version 5.1.0 from "
    echo "http://www.3gpp.org/ftp/Specs/latest/Rel-5/26_series/26073-510.zip"
    echo "and extracted src to libavcodec/amr"
    echo "You must also add -DMMS_IO and remove -pedantic-errors to/from CFLAGS in libavcodec/amr/makefile."
    echo "i.e. CFLAGS = -Wall -I. \$(CFLAGS_\$(MODE)) -D\$(VAD) -DMMS_IO"
    AMR_NB_FIXED=yes
  fi
])
AC_FF_ALLOW_ENABLE(AMR_WB, amr_wb, floating-point amr_wb audio codec,[
  echo "AMR WB FLOAT NOTICE ! Make sure you have downloaded TS26.204"
  echo "V5.1.0 from "
  echo "http://www.3gpp.org/ftp/Specs/archive/26_series/26.204/26204-510.zip"
  echo "and extracted the source to libavcodec/amrwb_float"
  AMR_WB=yes
])

dnl Check for video4linux1 headers (on Linux)
AC_FF_ALLOW_DISABLE(CONFIG_VIDEO4LINUX, v4l, video4linux support,[
  if test x$TARGET_OS = xlinux; then
    AC_CHECK_HEADER(linux/videodev.h, CONFIG_VIDEO4LINUX=yes,[
                    CONFIG_VIDEO4LINUX=no])
  else
    CONFIG_VIDEO4LINUX=no
  fi
])

dnl Check for BKTR (BSD variant of v4l)
AC_FF_ALLOW_DISABLE(CONFIG_BKTR, bktr, BSD/bktr support,[
  if test x$TARGET_OS = xfreebsd; then
    AC_CHECK_HEADER(dev/bktr/ioctl_bt848.h,CONFIG_BKTR=yes,[
      AC_CHECK_HEADER(machine/ioctl_bt848.h,CONFIG_BKTR=yes,[
        AC_CHECK_HEADER(dev/video/bktr/ioctl_bt848.h,CONFIG_BKTR=yes,[
          AC_CHECK_HEADER(dev/ic/bt8xx.h,CONFIG_BKTR=yes,CONFIG_BKTR=no)
        ])
      ])
    ])
  else
    CONFIG_BKTR=no
  fi
])

dnl Check for OSS headers
AC_FF_ALLOW_DISABLE(CONFIG_AUDIO_OSS, audio-oss, OSS audio support,[
  AC_CHECK_HEADER(sys/soundcard.h, CONFIG_AUDIO_OSS=yes, CONFIG_AUDIO_OSS=no)
])

dnl Check for DV (IEEE 1394 video)
AC_FF_ALLOW_DISABLE(CONFIG_DV1394, dv1394, DV IEEE1394 support,[
  if test x$TARGET_OS = xlinux; then
    CONFIG_DV1394=yes
  else
    CONFIG_DV1394=no
  fi
])

dnl Beos Audio
AC_FF_ALLOW_DISABLE(CONFIG_AUDIO_BEOS, audio-beos, BeOS audio support,[
  if test x$TARGET_OS = xbeos; then
    AC_LANG_CPLUSPLUS
    AC_CHECK_HEADER(SoundPlayer.h,[
      OLD_CXXFLAGS="$CXXFLAGS"
      CXXFLAGS="$CXXFLAGS -lbe -lmedia"
      AC_MSG_CHECKING(For BeOS audio libraries)
      AC_TRY_COMPILE([
#include <Application.h>
#include <SoundPlayer.h>
        ],[
          BSoundPlayer *p = new BSoundPlayer(NULL,NULL,NULL);
        ],[ CONFIG_AUDIO_BEOS=yes && AC_MSG_RESULT(yes) ],[
          CONFIG_AUDIO_BEOS=no && AC_MSG_RESULT(no)
      ])
      CXXFLAGS="$OLD_CXXFLAGS"
    ], CONFIG_AUDIO_BEOS=no)
    AC_LANG_C
  else
    CONFIG_AUDIO_BEOS=no
  fi
])

dnl Check for freetype2, used in vook/libdrawtext.so
AC_CHECK_FT2(2.0.9, HAVE_FT2=yes, HAVE_FT2=no)
AM_CONDITIONAL(HAVE_FREETYPE2, test "x$HAVE_FT2" = "xyes")

dnl Check for imlib2, used in vhook/libimlib2.so
dnl AM_PATH_IMLIB(2.0.0, HAVE_IMLIB2=yes, HAVE_IMLIB2=no)
HAVE_IMLIB2=no
AM_CONDITIONAL(HAVE_IMLIB2, test "x$HAVE_IMLIB2" = "xyes")

dnl Check for SDL, used in ffplay, and ffserver
AC_FF_ALLOW_DISABLE(CONFIG_FFPLAY, ffplay, ffmpeg media player,[
  AM_PATH_SDL(, CONFIG_FFPLAY=yes, CONFIG_FPFLAY=no)
])

dnl media server or not?
AC_FF_ALLOW_DISABLE(CONFIG_FFSERVER, ffserver, ffmpeg media server,[
  if test x$CONFIG_NETWORK = xyes -a x$CONFIG_ENCODERS = xyes; then
    CONFIG_FFSERVER=yes
  else
    CONFIG_FFSERVER=no
  fi
])

dnl main ffmpeg binary
AC_FF_ALLOW_DISABLE(CONFIG_FFMPEG, ffmpeg, ffmpeg command line utility,[
  if test x$CONFIG_ENCODERS = xyes; then
    CONFIG_FFMPEG=yes
    AC_DEFINE([FFMPEG_CONFIGURATION],"GStreamer","ffmpeg configure options")
  else
    CONFIG_FFMPEG=no
  fi
])

dnl docs
#AC_PATH_PROG(TEXI2HTML, texi2html)
#AM_CONDITIONAL(HAVE_TEXI2HTML, test x$TEXI2HTML != x)
#AC_PATH_PROG(POD2MAN, pod2man)
#AM_CONDITIONAL(HAVE_POD2MAN, test x$POD2MAN != x)

AC_OUTPUT([
Makefile
doc/Makefile
libavutil/Makefile
libavcodec/Makefile
libavcodec/alpha/Makefile
libavcodec/armv4l/Makefile
libavcodec/i386/Makefile
libavcodec/liba52/Makefile
libavcodec/libpostproc/Makefile
libavcodec/mlib/Makefile
libavcodec/ppc/Makefile
libavcodec/ps2/Makefile
libavcodec/sh4/Makefile
libavcodec/sparc/Makefile
libavformat/Makefile
tests/Makefile
vhook/Makefile
])

