# Makefile.am

AM_LDFLAGS = -Wl,--gc-sections -Wl,--as-needed
AM_CFLAGS = ${my_CFLAGS} \
	-fvisibility=hidden \
	-ffunction-sections \
	-fdata-sections

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
	can_lin.c  		\
	libbaalue.h 		\
	gettext.h

libbaalue_la_CFLAGS   = $(AM_CFLAGS)
libbaalue_la_LDFLAGS  = $(LIBCAPNG_LIBS) $(AM_LDFLAGS) -no-undefined -version-info 1:0:0
libbaalue_la_LIBADD   = $(LTLIBINTL) $(LIBCAPNG_LIBS)
