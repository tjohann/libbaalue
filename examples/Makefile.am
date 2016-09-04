# Makefile.am

EXTRA_DIST      = example01.dat
noinst_PROGRAMS = time_triggert_uds

AM_CPPFLAGS     = -I$(top_srcdir)/src
AM_LDFLAGS      = -L../src/

LIBS            = $(LTLIBINTL) $(SHM_LIBS)
LDADD           = ../src/libbaalue.la
