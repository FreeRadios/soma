EXTRA_DIST = config.m4 README test.php php.c

if ENABLE_PHPSOMA

if ENABLE_PHP4SOMA
all4=echo -e "\n\nCompiling phpsoma (php4)...\n\n"; \
     DIR=`pwd`; \
     if test @phptmpdir@/phpsoma4; then \
       rm -rf @phptmpdir@/phpsoma4; \
     fi; \
     mkdir -p @phptmpdir@/phpsoma4 && \
     cp php.c @phptmpdir@/phpsoma4 && \
     cp config.m4 @phptmpdir@/phpsoma4 && \
     cd @phptmpdir@/phpsoma4 && \
     phpize && \
     CFLAGS="$$CFLAGS -I$$DIR/../include -DPHPSOMA_INTERNAL" LIBS="$$LIBS -L$$DIR/../lib/.libs" ./configure && \
     make

install4=echo -e "\n\nInstalling phpsoma (php4)...\n\n"; \
         cd @phptmpdir@/phpsoma4 && \
         if test "@phpdir@" = ""; then \
	   make install; \
         else \
           INSTALL_ROOT=@phpdir@ make install; \
         fi 
else
all4=echo -e "\n\nSkipping phpsoma (php4)...\n\n"
install4=echo -e "\n\nSkipping phpsoma (php4)...\n\n"
endif

if ENABLE_PHP5SOMA
all5=echo -e "\n\nCompiling phpsoma (php5)...\n\n"; \
     DIR=`pwd`; \
     if test @phptmpdir@/phpsoma5; then \
       rm -rf @phptmpdir@/phpsoma5; \
     fi; \
     mkdir -p @phptmpdir@/phpsoma5 && \
     cp php.c @phptmpdir@/phpsoma5 && \
     cp config.m4 @phptmpdir@/phpsoma5 && \
     cd @phptmpdir@/phpsoma5 && \
     phpize5 && \
     CFLAGS="$$CFLAGS -I$$DIR/../include -DPHPSOMA_INTERNAL" LIBS="$$LIBS -L$$DIR/../lib/.libs" ./configure && \
     make

install5=echo -e "\n\nInstalling phpsoma (php5)...\n\n"; \
         cd @phptmpdir@/phpsoma5 && \
         if test "@phpdir@" = ""; then \
	   make install; \
         else \
           INSTALL_ROOT=@phpdir@ make install; \
         fi 
else
all5=echo -e "\n\nSkipping phpsoma (php5)...\n\n"
install5=echo -e "\n\nSkipping phpsoma (php5)...\n\n"
endif

all-local:
	($(all4)) && ($(all5))

install:
	($(install4)) && ($(install5))
endif
