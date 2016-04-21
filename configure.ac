# configure.ac

AC_INIT([libbaalue], [trunk], [thorsten.johannvorderbrueggen@t-online.de])
AC_COPYRIGHT([Copyright 2016, Thorsten Johannvorderbrueggen])

AC_CONFIG_AUX_DIR([build-aux/autoconf])
AC_CONFIG_MACRO_DIR([build-aux/m4])

AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR(src/error.c)

AC_CONFIG_FILES([Makefile \
		 src/Makefile])

#AM_GNU_GETTEXT_VERSION([0.19.4])
#AM_GNU_GETTEXT([external])

AM_INIT_AUTOMAKE([-Wall -Werror dist-bzip2])

# checks for programs.
AC_PROG_CC
AM_PROG_AR
AM_PROG_LEX

LT_INIT([
	disable-static
	pic-only
])

# -> ./configure --enable-silent-rules to get quite build behaviour
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([no])])

# ------

# checks for programs.
PKG_PROG_PKG_CONFIG

# checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([sys/stat.h])

# checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# checks for library functions.
AC_CHECK_FUNCS([strcasecmp strdup strndup])

AC_OUTPUT
