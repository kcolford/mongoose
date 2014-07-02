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

# Set the default prefix to /usr.  We do this because that is where
# glibc is installed on most systems and thus the compiler needs to be
# installed there too.
AC_PREFIX_DEFAULT([/usr])

# Determine the target system.
AC_CANONICAL_TARGET
AS_IF([test "$target_cpu" != "x86_64"], [
  AC_MSG_ERROR([Invalid target CPU.])
])
AS_IF([test "$target_os" != "linux-gnu"], [
  AC_MSG_ERROR([Invalid target OS.])
])

# Begin AutoMake intialization.
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

# Locate the AutoGen program for maintainer's use.
AC_ARG_VAR([AUTOGEN], [*Maintainers Only*: The AutoGen program to use,
must be version 5 or later.])
AC_CHECK_PROG([AUTOGEN], [autogen], [false])

# Locate Doxygen on the host system.
AC_ARG_VAR([DOXYGEN], [The instance of Doxygen to use when generating
documentation from source files.  Note: must be version 1.8 or higher])
AC_CHECK_PROG([DOXYGEN], [doxygen])
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

# Locate LaTeX for converting Doxygen output to PDFs.
AC_ARG_VAR([LATEX], [The instance of LaTeX to use when compiling .tex files.])
AC_CHECK_PROG([LATEX], [latex])
AS_IF([test -n "$LATEX"], [
  AC_SUBST([GENERATE_LATEX], [YES])
  AC_CHECK_PROG([PDFLATEX], [pdflatex])
  AS_IF([test -n "$PDFLATEX"], [
    AC_SUBST([USE_PDFLATEX], [YES])
  ], [
    AC_SUBST([USE_PDFLATEX], [NO])
  ])
  AC_CHECK_PROG([MAKEINDEX], [makeindex])
], [
  AC_SUBST([GENERATE_LATEX], [NO])
  LATEX=:
])

# Check if we can use Help2Man to create a man page for the user.
AS_IF([test $cross_compiling = no], [
  AM_MISSING_PROG([HELP2MAN], [help2man])
], [
  HELP2MAN=:
])

# Locate the Dynamic Linker by searching through the shell script
# ldd(1) that's distributed with glibc.
AC_PATH_PROG([LDD], [ldd])
AS_IF([test "$LDD"], [
  AC_PROG_GREP
  eval `$GREP '^RTLDLIST=".*"$' $LDD`
], [
  RTLDLIST=''
])
dynamiclinker=''
for d in $RTLDLIST
do
  AS_IF([test -r "$d"], [
    dynamiclinker="$d"
  ])
done
# Define the path for the dynamic linker that we will use.
AC_DEFINE_UNQUOTED([DYNAMIC_LINKER], ["$dynamiclinker"],
                   [Define to the path of the dynamic linker to use.])

# Determine the directory for glibc files.  This will be defined for
# use in the makefile.
AC_SUBST([archdir], ["${prefix}/lib/${target_cpu}-${target_os}"])

# Determine if we can in fact use glibc files on our own.  If not,
# we fall back on the existing compiler's ability to do this.
AC_CHECK_FILES([$archdir/crti.o $archdir/crt1.o $archdir/crtn.o], [
  AC_DEFINE([RUN_INDEPENDANTLY], [1],
            [Define to 1 if we are linking on our own.])
])

# BOOTSTRAP_PROG(VARIABLE, PROGS-TO-CHECK-FOR, [VARIABLE-COMMENT])
# ----------------------------------------------------------------
AC_DEFUN([BOOTSTRAP_PROG], [
  AC_CHECK_TARGET_TOOLS([_BOOTSTRAP_$1], [$2])
  AS_IF([test -n "$_BOOTSTRAP_$1"], [
    AC_DEFINE_UNQUOTED([$1], ["$_BOOTSTRAP_$1"], [$3])
  ])
])
# Check for programs that we can boostrap off of.
BOOTSTRAP_PROG([CC], [$CC], [The C compiler to bootstrap off of.])
BOOTSTRAP_PROG([CPP], [cpp], [The C preprocessor to boostrap off of.])
BOOTSTRAP_PROG([AS], [as gas], [The assembler to bootstrap off of.])
BOOTSTRAP_PROG([LD], [ld gold], [The linker to bootstrap off of.])

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

AM_GNU_GETTEXT_VERSION([0.19.1])
AM_GNU_GETTEXT([external])

AC_CONFIG_FILES([Makefile
		 doc/doc.cfg
		 doc/Makefile
		 lib/Makefile
		 man/Makefile
		 po/Makefile.in
		 src/Makefile
		 tests/Makefile
		 tests/working-programs/Makefile])
AC_OUTPUT
