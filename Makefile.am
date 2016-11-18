# Makefile.am

CLEANFILES = *~ '\#*\#'

ACLOCAL_AMFLAGS  = -I build-aux/m4

EXTRA_DIST = build-aux/autoconf/config.rpath README.md
doc_DATA = README.md AUTHORS COPYING

SED_PROCESS = \
	$(AM_V_GEN)$(MKDIR_P) $(dir $@) && $(SED) \
	-e 's,@VERSION\@,$(VERSION),g' \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@exec_prefix\@,$(exec_prefix),g' \
	-e 's,@libdir\@,$(libdir),g' \
	-e 's,@includedir\@,$(includedir),g' \
	< $< > $@ || rm $@

%.pc: %.pc.in Makefile
	$(SED_PROCESS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libbaalue.pc
CLEANFILES += src/libbaalue.pc

EXAMPLES         =
if ENABLE_EXAMPLES
  EXAMPLES      += examples
endif

MYSCRIPTS    	=
if ENABLE_SCRIPTS
  MYSCRIPTS   	+= scripts
endif

SUBDIRS         = src po $(EXAMPLES) man $(MYSCRIPTS) Documentation
