# Makefile.am

AM_LDFLAGS = -Wl,--gc-sections -Wl,--as-needed
AM_CFLAGS = ${my_CFLAGS} \
	-fvisibility=hidden \
	-ffunction-sections \
	-fdata-sections

SRC_FILES = error.c \
	helper.c \
	wrapper.c \
	network.c \
	sh_mem.c \
	sh_mq.c	\
	process.c \
	fiber.c \
	serialize.c \
	can_lin.c \
	libbaalue.h \
	gettext.h

HDR_FILES = libbaalue.h

if ENABLE_LCD160X
SRC_FILES += lcd160x.c \
	libbaalue_lcd160x.h
HDR_FILES += libbaalue_lcd160x.h
endif

include_HEADERS = $(HDR_FILES)
libbaalue_la_SOURCES = $(SRC_FILES)
lib_LTLIBRARIES = libbaalue.la

libbaalue_la_CFLAGS   = $(AM_CFLAGS)
libbaalue_la_LDFLAGS  = $(LIBCAPNG_LIBS) $(AM_LDFLAGS) -version-info $(LIBBAALUE_CURRENT):$(LIBBAALUE_REVISION):$(LIBBAALUE_AGE)
libbaalue_la_LIBADD   = $(LTLIBINTL) $(LIBCAPNG_LIBS)
