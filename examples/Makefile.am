# Makefile.am

EXTRA_DIST      = example01.dat
noinst_PROGRAMS = time_triggert_uds 		\
		  time_triggert_uds_server	\
		  lcd1602			\
		  can_send 			\
		  can_recv 			\
		  udp_mgmt_server		\
		  udp_mgmt_ping_client		\
		  udp_mgmt_reboot_client	\
		  user_group_info		\
                  inet_daytime_client  		\
		  inet_daytime_server  		\
		  time_triggert_simple

AM_CFLAGS     	= -I$(top_srcdir)/src $(my_CFLAGS) -Wno-sign-compare
AM_LDFLAGS      = -L../src/

LIBS            = $(LTLIBINTL)
LDADD           = ../src/libbaalue.la
