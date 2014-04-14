EXTRA_DIST = $(TESTS) tester.sh
MOSTLYCLEANFILES = core log-*

COMPILER = "$(VALGRIND) $(top_builddir)/src/compiler"

TESTS_ENVIRONMENT = \
env CC=$(CC) COMPILER=$(COMPILER) $(SHELL)

TESTS =						\
prog-1						\
prog-2						\
prog-3						\
prog-4						\
prog-5						\
prog-6						\
prog-7						\
prog-8						\
prog-9

#XFAIL_TESTS = prog-8
