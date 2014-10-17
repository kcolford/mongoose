# BOOTSTRAP_PROG(VARIABLE, PROGS-TO-CHECK-FOR, [VARIABLE-COMMENT])
# ----------------------------------------------------------------

#serial 1

AC_DEFUN([BOOTSTRAP_PROG], [
  AC_PREREQ([2.68])
  AC_CHECK_TARGET_TOOLS([_BOOTSTRAP_$1], [$2])
  AS_IF([test "$_BOOTSTRAP_$1"], [
    AC_DEFINE_UNQUOTED([$1], ["$_BOOTSTRAP_$1"], [$3])
  ])
])
