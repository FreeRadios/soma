EXTRA_DIST = python.c test.py setup.py setup.py.in README

if ENABLE_PYSOMA
all-local:
	python setup.py build;

install:
	if test "@pythondir@" = ""; then \
	  python setup.py install; \
	else \
	  python setup.py install --home=@pythondir@; \
        fi;

endif
