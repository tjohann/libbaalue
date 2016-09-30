# Makefile.am

EXTRA_DIST      = example01.dat
noinst_PROGRAMS = time_triggert_uds 		\
		  time_triggert_uds_server	\
		  can_send 			\
		  can_recv 			\
		  udp_mgmt_server		\
		  udp_mgmt_client		\
                  inet_daytime_client  		\
		  inet_daytime_server  		\
		  time_triggert_simple		\
		  time_triggert_udp		\
		  time_triggert_udp_server

AM_CFLAGS     	= -I$(top_srcdir)/src $(my_CFLAGS)
AM_LDFLAGS      = -L../src/

LIBS            = $(LTLIBINTL)
LDADD           = ../src/libbaalue.la
