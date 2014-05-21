#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([Compiler],
        [m4_esyscmd([build-aux/git-version-gen .tarball-version])],
	[colfordk@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/compiler.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_USE_SYSTEM_EXTENSIONS
AM_INIT_AUTOMAKE([foreign silent-rules color-tests dist-xz])
LT_INIT

# Checks for programs.
AC_PROG_CC
gl_EARLY
gl_INIT
AC_PROG_CXX
AC_PROG_YACC
AM_PROG_LEX
AC_PROG_RANLIB

AM_MISSING_PROG([AUTOGEN], [autogen])

AX_COMPARE_VERSION([1.8], [le], [$(doxygen -v)], [AS_ECHO(
['checking for doxygen version 1.8 or higher... yes']
)], [AC_MSG_ERROR(
[you need Doxygen version 1.8 or higher to generate the documentation.])])

AS_IF([test $cross_compiling = no], [
  AM_MISSING_PROG([HELP2MAN], [help2man])
], [
  HELP2MAN=:
])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h libintl.h locale.h stddef.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_CHECK_FUNCS([atexit memset setlocale strtol])
AC_FUNC_ALLOCA
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_DEFINE([COPYRIGHT_YEAR], [m4_esyscmd([date +%Y])],
          [year in copyright message])

AC_DEFINE_UNQUOTED([C_COMPILER], ["$CC"],
		   [C compiler used to bootstrap this project])

AM_GNU_GETTEXT_VERSION([0.18.1])
AM_GNU_GETTEXT([external])

AC_CONFIG_FILES([doc.cfg
		 Makefile
		 lib/Makefile
		 man/Makefile
		 po/Makefile.in
		 src/Makefile
		 tests/Makefile])
AC_OUTPUT
