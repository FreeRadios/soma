AC_PREREQ(2.54)

AC_INIT(daemon/main.c)
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AC_PROG_MAKE_SET

AM_SANITY_CHECK
AM_INIT_AUTOMAKE(soma,2.4)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_LN_S
AC_ISC_POSIX
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AM_PROG_CC_STDC

AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_MODE_T
AC_TYPE_SIGNAL
AC_HEADER_TIME

AC_HEADER_STDC
AC_HEADER_DIRENT
AC_CHECK_HEADERS(unistd.h stdlib.h stdio.h string.h sys/types.h sys/stat.h dirent.h signal.h sys/wait.h time.h pwd.h grp.h fcntl.h sys/param.h sys/socket.h netinet/in.h arpa/inet.h netdb.h termios.h sched.h errno.h)

AC_FUNC_VPRINTF

AC_CHECK_FUNCS(gettimeofday select socket strdup nanosleep inet_aton scandir getcwd mktime strerror sethostent endhostent getnameinfo getaddrinfo)

AC_CHECK_FILE(/proc/1/cmdline)
AH_VERBATIM([HAVE_PROC],[#undef HAVE_PROC])
if test $ac_cv_file__proc_1_cmdline = yes; then
  AC_DEFINE(HAVE_PROC)
fi

AC_ARG_ENABLE(sm, AC_HELP_STRING([--enable-sm],[enable libsm - simple mutex (default=no)]), sm=$enableval, sm=no)
AH_VERBATIM([ENABLE_SM],[#undef ENABLE_SM])
if test $sm = yes; then
  AC_DEFINE(ENABLE_SM)

  case "$host_os" in
    darwin*) CFLAGS="$CFLAGS -Dppc"; ;;
    powerpc*) CFLAGS="$CFLAGS -Dppc"; ;;
    sparc*) CFLAGS="$CFLAGS -Dppc"; ;;
  esac
fi
AM_CONDITIONAL(ENABLE_SM, test "$sm" = yes)

AC_CHECK_HEADER(pthread.h,, AC_MSG_ERROR(pthread is required to compile soma))

AC_CHECK_LIB(pthread,pthread_mutex_init,, AC_MSG_ERROR(pthread is required to compile soma))

AC_CHECK_HEADER(dlfcn.h)
AC_CHECK_FUNC(dlopen, , 
  AC_CHECK_LIB(dl, dlopen, ,
    AC_MSG_ERROR(dynamic load support is required to compile soma)
  )
)

AC_CHECK_HEADER(confuse.h,
  AC_CHECK_LIB(confuse, cfg_init, NEED_CONFUSE=no, NEED_CONFUSE=yes),
  NEED_CONFUSE=yes
)

AH_VERBATIM([ENABLE_CONFUSE],[#undef ENABLE_CONFUSE])

if test "$NEED_CONFUSE" = yes; then
  AC_DEFINE(ENABLE_CONFUSE, 1, [Define this symbol if you have libconfuse])
else
  LIBS="$LIBS -lconfuse"
fi

AM_CONDITIONAL(ENABLE_CONFUSE, test "$NEED_CONFUSE" = yes)

AC_CHECK_LIB(ncurses,tgetstr,, AC_MSG_ERROR(libncurses is required to compile soma))

AC_CHECK_HEADER(readline/readline.h,,AC_MSG_ERROR(libreadline is required to compile soma))
AC_CHECK_LIB(readline,rl_newline,, AC_MSG_ERROR(libreadline is required to compile soma))

AC_CHECK_HEADER(readline/history.h,,AC_MSG_ERROR(libreadline is required to compile soma))
AC_CHECK_LIB(history,using_history,, AC_MSG_ERROR(libreadline is required to compile soma))

AC_ARG_ENABLE(openssl, AC_HELP_STRING([--enable-openssl],[enable ssl extensions (default=yes)]), openssl=$enableval, openssl=yes)
if test "$openssl" = yes; then
  AC_PATH_PROG(pkgconfigpath, pkg-config)
  AC_MSG_CHECKING(for openssl through pkg-config)
  if test "$pkgconfigpath" != "" && \
     $pkgconfigpath openssl --exists; then
    CFLAGS="$CFLAGS `$pkgconfigpath openssl --cflags` -DSOMA_USE_OPENSSL"
    LIBS="$LIBS `$pkgconfigpath openssl --libs`"
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(No openssl support)
    openssl=no
  fi
fi

PKG_CHECK_MODULES(CHECK_XML, libxml-2.0 >= 2.0.0,, AC_MSG_ERROR(libxml2 is requred to compile soma))
LDFLAGS="$LDFLAGS $CHECK_XML_LIBS"
CFLAGS="$CFLAGS $CHECK_XML_CFLAGS"

AC_CHECK_FILE(/dev/null)
if test $ac_cv_file__dev_null = no; then
  AC_MSG_ERROR(soma needs /dev/null!)
fi

AC_MSG_CHECKING([if argv can be written to])
AC_CACHE_VAL(soma_cv_writable_argv, [
  AC_RUN_IFELSE([[
   int main(int argc, char ** argv) {
     argv[0][0]= ' ';
     return (argv[0][0] == ' ') ? 0 : 1;
   }
   ]],
   soma_cv_writable_argv=yes,
   soma_cv_writable_argv=no,
   soma_cv_writable_argv=cross)
])

case $soma_cv_writable_argv in
  yes)
    AC_DEFINE(HAVE_WRITABLE_ARGV, 1, [Define this symbol if your OS supports changing the contents of argv])
    AC_MSG_RESULT(yes)
    ;;
  no)
    AC_MSG_RESULT(no)
    ;;
  *)
    AC_MSG_RESULT(no)
    AC_MSG_WARN([the previous check could not be made default was used])
    ;;
esac

#### FFMPEG check... ###
AC_ARG_ENABLE(ffmpeg, AC_HELP_STRING([--enable-ffmpeg],[enable external ffmpeg (default=no)]), ffmpeg=$enableval, ffmpeg=no)
AH_VERBATIM([ENABLE_FFMPEG],[#undef ENABLE_FFMPEG])
AM_CONDITIONAL(ENABLE_FFMPEG, test "$ffmpeg" = yes)

if test $ffmpeg = yes; then
  AC_DEFINE(ENABLE_FFMPEG)

  # Normal mode:
  AC_CHECK_HEADER(ffmpeg/avformat.h,,AC_MSG_ERROR(libavformat is required to compile soma))
  AC_CHECK_LIB(avformat,av_open_input_file, HAVE_AVFORMAT=yes, HAVE_AVFORMAT=no)
  
  # Avformat only ?
  if test "$HAVE_AVFORMAT" = yes; then
    LDFLAGS="$LDFLAGS -lavformat"
  fi
  
  # Avformat + avcodec ?
  if test "$HAVE_AVFORMAT" = no; then
    AC_CHECK_LIB(avcodec,av_malloc, HAVE_AVCODEC=yes, HAVE_AVCODEC=no)
  
    if test "$HAVE_AVCODEC" = yes; then
      LDFLAGS_OLD="$LDFLAGS"
      LDFLAGS="$LDFLAGS -lavcodec"
  
      AC_CHECK_LIB(avformat,av_find_stream_info,
                   HAVE_AVFORMAT=yes, HAVE_AVFORMAT=no)
  
      if test "$HAVE_AVFORMAT" = no; then
        LDFLAGS="$LDFLAGS_OLD"
      else
        LDFLAGS="$LDFLAGS -lavformat"
      fi
    fi
  fi
  
  # ffmpeg-config ? (debian)
  if test "$HAVE_AVFORMAT" = no; then
  
    # ffmpeg-config mode:
    AC_PATH_PROG(ffmpegconfigpath, ffmpeg-config)
    AC_MSG_CHECKING(for ffmpeg through ffmpeg-config)
    if test "$ffmpegconfigpath" = ""; then
      HAVE_AVFORMAT=no
  
    else
      HAVE_AVFORMAT=yes
  
      AVFORMAT_LIBS=`$ffmpegconfigpath --libs avformat`
      LIBS="$LIBS $AVFORMAT_LIBS"
  
      AVFORMAT_CFLAGS=`$ffmpegconfigpath --cflags`
      CFLAGS="$CFLAGS $AVFORMAT_CFLAGS"
    fi
  
    AC_MSG_RESULT($HAVE_AVFORMAT)
  fi
  
  # pkg-config ? (source)
  if test $HAVE_AVFORMAT = no; then
    PKG_CHECK_MODULES(CHECK_AVFORMAT, libavformat,
                      HAVE_AVFORMAT=yes, HAVE_AVFORMAT=no)
  
    if test $HAVE_AVFORMAT = yes; then
      LDFLAGS="$LDFLAGS $CHECK_AVFORMAT_LIBS"
      CFLAGS="$CFLAGS $CHECK_AVFORMAT_CFLAGS"
    fi
  fi
  
  if test $HAVE_AVFORMAT = no; then
    AC_MSG_ERROR(ffmpeg is required to compile soma)
  fi
else
  ac_configure_args="$ac_configure_args --disable-v4l --disable-audio-oss --disable-dv1394 --disable-vhook --disable-ffmpeg --disable-ffserver --disable-ffplay --enable-pp"
  AC_CONFIG_SUBDIRS(ffmpeg/ffmpeg)
fi

## PHPTMPDIR ##
AC_ARG_WITH(phptmpdir, AC_HELP_STRING([--with-phptmpdir=[where/you/have/a/tmp/dir]],[a tmp directory for the compiling procedure of the php extension]), 
  AC_SUBST(phptmpdir, [$withval]),
  AC_SUBST(phptmpdir, [/tmp])
)

## PHPDIR ##
AC_ARG_WITH(phpdir, AC_HELP_STRING([--with-phpdir=[where/you/want/the/php/installation]],[set the directory for the python installing procedure (empty means the default php dir)]), 
  AC_SUBST(phpdir, [$withval])
)

## PYTHONDIR ##
AC_ARG_WITH(pythondir, AC_HELP_STRING([--with-pythondir=[where/you/want/the/python/installation]],[set the directory for the python installing procedure (empty means the default python dir)]), 
  AC_SUBST(pythondir, [$withval])
)

## EXTENSIONS ##
AC_ARG_ENABLE(phpsoma, AC_HELP_STRING([--enable-phpsoma],[enable the php extension (default=no)]), phpsoma=$enableval, phpsoma=no)
AH_VERBATIM([ENABLE_PHPSOMA],[#undef ENABLE_PHPSOMA])
AH_VERBATIM([ENABLE_PHP5SOMA],[#undef ENABLE_PHP5SOMA])
AH_VERBATIM([ENABLE_PHP4SOMA],[#undef ENABLE_PHP4SOMA])

AM_CONDITIONAL(ENABLE_PHPSOMA, test "$phpsoma" = yes)

if test $phpsoma = yes; then
  AC_PATH_PROG(php5soma, phpize5)
  AC_PATH_PROG(php4soma, phpize)

  AM_CONDITIONAL(ENABLE_PHP5SOMA, test "$php5soma" != "")
  AM_CONDITIONAL(ENABLE_PHP4SOMA, test "$php4soma" != "")

  if test "$php5soma" = "" && \
     test "$php4soma" = ""; then
    AC_MSG_ERROR(phpize is required to compile phpsoma)
  fi
else
  AM_CONDITIONAL(ENABLE_PHP5SOMA, test "$php5soma" = yes)
  AM_CONDITIONAL(ENABLE_PHP4SOMA, test "$php4soma" = yes)
fi

AC_ARG_ENABLE(pysoma, AC_HELP_STRING([--enable-pysoma],[enable the py extension (default=no)]), pysoma=$enableval, pysoma=no)

AH_VERBATIM([ENABLE_PYSOMA],[#undef ENABLE_PYSOMA])
AM_CONDITIONAL(ENABLE_PYSOMA, test "$pysoma" = yes)

if test $pysoma = yes; then
  AC_CHECK_HEADER(python2.3/Python.h,python23=1)
  AC_CHECK_HEADER(python2.4/Python.h,python24=1)
  AC_CHECK_HEADER(python2.5/Python.h,python25=1)

  if test "$python23" = "" && \
     test "$python24" = "" && \
     test "$python25" = ""; then
    AC_MSG_ERROR(Python.h is required to compile pysoma)
  fi

  if test "$python25" = "1"; then
    CFLAGS="$CFLAGS -DPYSOMA25"
  elif test "$python24" = "1"; then
    CFLAGS="$CFLAGS -DPYSOMA24"
  elif test "$python23" = "1"; then
    CFLAGS="$CFLAGS -DPYSOMA23"
  fi

  AC_SUBST(pysoma_include, [`pwd`/include])
  AC_SUBST(pysoma_library, [`pwd`/lib/.libs])
  AC_SUBST(pysoma_internal, [1])
else
  AC_SUBST(pysoma_include, [..])
  AC_SUBST(pysoma_library, [.])
  AC_SUBST(pysoma_internal, [0])
fi

## END ##
CFLAGS="$CFLAGS -Wall"

AC_OUTPUT([
Makefile
daemon/Makefile
lib/Makefile
client/Makefile
check/Makefile
ghthash/Makefile
confuse/Makefile
ffmpeg/Makefile
sm/Makefile
include/Makefile
config/Makefile
config/soma-config.c
pysoma/Makefile
pysoma/setup.py
phpsoma/Makefile
])

