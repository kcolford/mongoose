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

AC_ARG_VAR([AUTOGEN], [*Maintainers Only*: The AutoGen program to use,
must be version 5 or later.])
AC_PATH_PROG([AUTOGEN], [autogen], [touch])

AC_ARG_VAR([DOXYGEN], [The instance of Doxygen to use when generating
documentation from source files.  Note: must be version 1.8 or higher])
AC_PATH_PROG([DOXYGEN], [doxygen])
AS_IF([test -n "$DOXYGEN"], [
  AC_MSG_CHECKING([whether doxygen is version 1.8 or higher])
  AX_COMPARE_VERSION([1.8], [le], [$($DOXYGEN -v)], [
    AC_MSG_RESULT([yes])
  ], [
    DOXYGEN=:
    AC_MSG_RESULT([no])
  ])
], [
  DOXYGEN=:
])

AC_ARG_VAR([LATEX], [The instance of LaTeX to use when compiling .tex files.])
AC_PATH_PROG([LATEX], [latex])
AS_IF([test -n "$LATEX"], [
  AC_SUBST([GENERATE_LATEX], [YES])
  AC_PATH_PROG([PDFLATEX], [pdflatex])
  AS_IF([test -n "$PDFLATEX"], [
    AC_SUBST([USE_PDFLATEX], [YES])
  ], [
    AC_SUBST([USE_PDFLATEX], [NO])
  ])
  AC_PATH_PROG([MAKEINDEX], [makeindex])
], [
  AC_SUBST([GENERATE_LATEX], [NO])
  LATEX=:
])

AS_IF([test $cross_compiling = no], [
  AM_MISSING_PROG([HELP2MAN], [help2man])
], [
  HELP2MAN=:
])

# Set up warnings for the compilation routines.
gl_MANYWARN_ALL_GCC([warnings])
nw=
nw="$nw -Wsystem-headers"	# Ridiculous warning.
nw="$nw -Wvla"			# Breaks gnulib gettext.h.
nw="$nw -Wswitch-default"	# Breaks generated files.
nw="$nw -Wunused-macros"	# Likewise.
gl_MANYWARN_COMPLEMENT([warnings], [$warnings], [$nw])
for w in $warnings; do
  gl_WARN_ADD([$w])
done

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h libintl.h locale.h stddef.h])
AC_HEADER_STDBOOL

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

AC_DEFINE_UNQUOTED([CC], ["$CC"], [C compiler used to bootstrap])

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
