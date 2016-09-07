# Makefile.am

EXTRA_DIST      = example01.dat
noinst_PROGRAMS = time_triggert_uds can_send can_recv uds_server uds_client

AM_CPPFLAGS     = -I$(top_srcdir)/src
AM_LDFLAGS      = -L../src/

LIBS            = $(LTLIBINTL) $(SHM_LIBS)
LDADD           = ../src/libbaalue.la
