from distutils.core import setup, Extension
from os import popen

ssl=0
f=popen("soma-config --cflags")
for i in f:
	if i.find("SOMA_USE_OPENSSL") != -1:
		ssl=1

pysoma = Extension (
	'pysoma',
	define_macros = [('SOMA_USE_OPENSSL', ssl),
	                 ('PYSOMA_INTERNAL', 0)],
	include_dirs = ['..'],
	library_dirs = ['.'],
	sources = ['python.c'],
	libraries = ['soma']
)

setup (
	name = "pysoma",
	version = "1.0",
	description = "Admin interface for somad",
	author = "Andrea Marchesini",
	author_email = "bakunin@autistici.org",
	url = "http://www.somasuite.org",
	ext_modules = [ pysoma ]
)

