# Makefile.am

if ENABLE_DEBUG_INFO
AM_CFLAGS = ${my_CFLAGS} \
	${my_DEBUG_CFLAGS} \
	-fvisibility=hidden \
	-ffunction-sections \
	-fdata-sections
else
AM_CFLAGS = ${my_CFLAGS} \
	-fvisibility=hidden \
	-ffunction-sections \
	-fdata-sections
endif
AM_LDFLAGS = -Wl,--gc-sections -Wl,--as-needed

SRC_FILES = error.c \
	helper.c \
	wrapper.c \
	network.c \
	sh_mem.c \
	sh_mq.c	\
	process.c \
	fiber.c \
	user.c \
	group.c \
	serialize.c \
	can_lin.c \
	device_mgmt.c \
	libbaalue_types.h \
	datatypes.c \
	libbaalue.h \
	gettext.h

HDR_FILES = libbaalue.h libbaalue_types.h

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
