#
# configure.ac
#

AC_INIT([libbaalue], [trunk], [thorsten.johannvorderbrueggen@t-online.de])
AC_COPYRIGHT([Copyright 2016, Thorsten Johannvorderbrueggen])

AC_CONFIG_AUX_DIR([build-aux/autoconf])
AC_CONFIG_MACRO_DIR([build-aux/m4])

AM_CONFIG_HEADER(config.h)
AC_CONFIG_FILES([Makefile		\
		src/Makefile		\
		po/Makefile.in		\
		examples/Makefile	\
		man/Makefile		\
		scripts/Makefile	\
		libbaalue.pc])

AM_GNU_GETTEXT_VERSION([0.19.8])
AM_GNU_GETTEXT([external])

AM_INIT_AUTOMAKE([-Wall dist-bzip2 silent-rules])

AC_PREFIX_DEFAULT([/usr])

# checks for programs
AC_PROG_SED
AC_PROG_CC
# use standard version -> @today:c99
#AC_PROG_CC_STDC
AM_PROG_AR
AC_PROG_MKDIR_P

# init libtool
LT_INIT([pic-only])

# ./configure --enable-silent-rules/--disable-silent-rules
AM_SILENT_RULES([yes])

# enable build of examples
AC_ARG_ENABLE([examples],
	AS_HELP_STRING([--enable-examples], [enable build examples @<:@default=disabled@:>@]),
	[], [enable_examples=no])
AM_CONDITIONAL([ENABLE_EXAMPLES], [test "$enable_examples" = yes])

# enable debug info
AC_ARG_ENABLE([debug],
        AS_HELP_STRING([--enable-debug], [enable debug messages @<:@default=disabled@:>@]),
        [], [enable_debug=no])
AM_CONDITIONAL([__DEBUG__], [test "$enable_debug" = yes])

# enable can scripts
AC_ARG_ENABLE([scripts],
        AS_HELP_STRING([--enable-scripts], [enable install of scripts @<:@default=disabled@:>@]),
        [], [enable_scripts=no])
AM_CONDITIONAL([ENABLE_SCRIPTS], [test "$enable_scripts" = yes])

# enable build of lcd160x parts
AC_ARG_ENABLE([lcd160x],
	AS_HELP_STRING([--enable-lcd160x], [enable build lcd160x functions @<:@default=disable@:>@]),
	[], [enable_lcd160x=no])
AM_CONDITIONAL([ENABLE_LCD160X], [test "$enable_lcd160x" = yes])

# my common CFLAGS
my_CFLAGS="\
-Wall \
-Wchar-subscripts \
-Wformat-security \
-Wmissing-declarations \
-Wmissing-prototypes \
-Wnested-externs \
-Wpointer-arith \
-Wshadow \
-Wsign-compare \
-Wstrict-prototypes \
-Wtype-limits
"
AC_SUBST([my_CFLAGS])

# ------

LIBBAALUE_CURRENT=1
AC_SUBST([LIBBAALUE_CURRENT])
LIBBAALUE_REVISION=0
AC_SUBST([LIBBAALUE_REVISION])
LIBBAALUE_AGE=0
AC_SUBST([LIBBAALUE_AGE])

# ------

# checks for more programs
PKG_PROG_PKG_CONFIG

# Checks for libraries.
PKG_CHECK_MODULES([LIBCAPNG], [libcap-ng])

# checks for header files
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])

# checks for typedefs, structures, and compiler characteristics
AC_C_CONST

# checks for library functions and more
AC_CHECK_FUNCS([strcasecmp strdup strndup])

AC_CHECK_LIB([rt], [shm_open],
			[],
			[AC_MSG_ERROR([** Could not find librt **])])
AC_CHECK_LIB([pthread], [pthread_create],
			[],
			[AC_MSG_ERROR([** Could not find libpthread **])])
AC_OUTPUT

# show some basic config info
AC_MSG_RESULT([
        $PACKAGE $VERSION
        =======================================

        prefix:                 ${prefix}
        sysconfdir:             ${sysconfdir}
        libdir:                 ${libdir}
        includedir:             ${includedir}

        compiler:               ${CC}
        cflags:                 ${CFLAGS}

        examples:               ${enable_examples}
	debug:                  ${enable_debug}
	scripts:		${enable_scripts}
	lcd160x:		${enable_lcd160x}
])
