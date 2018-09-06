AUTOMAKE_OPTIONS = subdir-objects foreign no-dependencies

bin_PROGRAMS=simplecgen
AM_CFLAGS=	-Wall -std=c99 -g
simplecgen_SOURCES=		\
	simplecgen.c	\
	utils.c	\
	template_functions.c

PACKAGE = @PACKAGE@
VERSION = @VERSION@

sysconfdir = @sysconfdir@

# This isn't used yet
DEFS = -DSYSCONFDIR=\"$(sysconfdir)\" @DEFS@

