# Makefile.am

lib_LTLIBRARIES = libbaalue.la
include_HEADERS = libbaalue.h

libbaalue_la_SOURCES  = error.c \
	helper.c 		\
	wrapper.c		\
	network.c		\
	sh_mem.c		\
	sh_mq.c			\
	process.c  		\
	fiber.c  		\
	serialize.c  		\
	libbaalue.h 		\
	gettext.h

libbaalue_la_LIBADD   = $(LTLIBINTL)
libbaalue_la_LDFLAGS  = $(AM_LDFLAGS) -no-undefined -version-info 1:0:0
