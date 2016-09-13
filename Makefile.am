# Makefile.am

CLEANFILES = *~ '\#*\#'

ACLOCAL_AMFLAGS  = -I build-aux/m4

EXTRA_DIST = build-aux/autoconf/config.rpath README.md
doc_DATA = README.md AUTHORS COPYING

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbaalue.pc

EXAMPLES         =
if ENABLE_EXAMPLES
  EXAMPLES      += examples
endif

MYSCRIPTS    	=
if ENABLE_SCRIPTS
  MYSCRIPTS   	+= scripts
endif

SUBDIRS         = src po $(EXAMPLES) man $(MYSCRIPTS)
