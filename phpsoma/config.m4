PHP_ARG_WITH(soma, for SOMA support,
[  --with-soma[=DIR]         Include SOMA support where DIR is SOMA install
                            prefix. If DIR is not set, the bundled SOMA library
                            will be used.])

PHP_ARG_ENABLE(soma, Support for somad,
[ --enable-soma    Enable soma support])

if test "$PHP_SOMA" = "yes"; then

  PHP_CHECK_LIBRARY(soma, soma_open_unix, [
      AC_DEFINE(HAVE_SOMA, 1, [ ])
    ],[
      AC_MSG_ERROR([Soma library required.])
    ], [
      -L$SOMA_DIR/lib
    ]
   )

  PHP_ADD_LIBRARY(soma, 1, SOMA_SHARED_LIBADD)
  PHP_NEW_EXTENSION(soma, php.c, $ext_shared)
  PHP_SUBST(SOMA_SHARED_LIBADD)
fi
