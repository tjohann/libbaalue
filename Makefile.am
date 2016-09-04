# Makefile.am

CLEANFILES = *~ '\#*\#'

AM_LDFLAGS = -Wl,--gc-sections -Wl,--as-needed
AM_CFLAGS = -Wall 		\
	-Wchar-subscripts 	\
	-Wformat-security 	\
	-Wmissing-declarations 	\
	-Wmissing-prototypes 	\
	-Wnested-externs 	\
	-Wpointer-arith 	\
	-Wshadow 		\
	-Wsign-compare 		\
	-Wstrict-prototypes 	\
	-Wtype-limits 		\
	-fvisibility=hidden 	\
	-ffunction-sections 	\
	-fdata-sections

ACLOCAL_AMFLAGS  = -I build-aux/m4
EXTRA_DIST = build-aux/autoconf/config.rpath README.md COPYING
doc_DATA = README.md AUTHORS COPYING

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbaalue.pc

EXAMPLES         =
if ENABLE_EXAMPLES
  EXAMPLES      += examples
endif
SUBDIRS          = src po $(EXAMPLES)
