EXTRA_DIST = $(TESTS) tester.sh
MOSTLYCLEANFILES = core *.log

COMPILER = $(VALGRIND) $(top_builddir)/src/compiler

ENVIRONMENT = COMPILER="$(COMPILER)" CC="$(CC)"	\
  $(TIMEOUT) --foreground --signal KILL 10s

TEST_EXTENSIONS = .c
C_LOG_COMPILER = $(ENVIRONMENT) $(SHELL) $(srcdir)/tester.sh

TESTS =						\
prog-1.c					\
prog-2.c					\
prog-3.c					\
prog-4.c					\
prog-5.c					\
prog-6.c					\
prog-7.c					\
prog-8.c					\
prog-9.c					\
prog-10.c					\
prog-11.c					\
prog-12.c					\
prog-13.c					\
prog-14.c					\
prog-15.c					\
prog-16.c					\
prog-17.c					\
prog-18.c					\
prog-gcd.c					\
prog-primes.c

#XFAIL_TESTS = prog-8.c
