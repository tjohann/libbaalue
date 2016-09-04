# configure.ac

AC_INIT([libbaalue], [trunk], [thorsten.johannvorderbrueggen@t-online.de])
AC_COPYRIGHT([Copyright 2016, Thorsten Johannvorderbrueggen])

AC_CONFIG_AUX_DIR([build-aux/autoconf])
AC_CONFIG_MACRO_DIR([build-aux/m4])

AM_CONFIG_HEADER(config.h)

AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in examples/Makefile libbaalue.pc])

AM_GNU_GETTEXT_VERSION([0.19.7])
AM_GNU_GETTEXT([external])

AM_INIT_AUTOMAKE([-Wall dist-bzip2])

# checks for programs.
AC_PROG_CC
AM_PROG_AR
AM_PROG_LEX
AC_PROG_CC_STDC

LT_INIT
#LT_INIT([
#	disable-static
#	pic-only
#])

# -> ./configure --enable-silent-rules to get quite build behaviour
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([no])])

# disable build of examples
AC_ARG_ENABLE([examples],
	[AC_HELP_STRING([--disable-examples], [don't build examples])],
	[], [enable_examples=yes])
AM_CONDITIONAL([ENABLE_EXAMPLES], [test "$enable_examples" = yes])

# ------

# checks for programs.
PKG_PROG_PKG_CONFIG

# Checks for libraries.
PKG_CHECK_MODULES([LIBCAPNG], [libcap-ng])
PKG_CHECK_MODULES([LIBGIT2], [libgit2])
PKG_CHECK_MODULES([LIBCURL], [libcurl])

# checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])

# checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# checks for library functions.
AC_CHECK_FUNCS([strcasecmp strdup strndup])

AC_CHECK_LIB([rt], [shm_open], [SHM_LIBS="-lrt"], [SHM_LIBS=""])
AC_SUBST(SHM_LIBS)

AC_OUTPUT

AC_MSG_RESULT([
        $PACKAGE $VERSION
        =======================================

        prefix:                 ${prefix}
        sysconfdir:             ${sysconfdir}
        libdir:                 ${libdir}
        includedir:             ${includedir}

        compiler:               ${CC}
        cflags:                 ${CFLAGS}
])
