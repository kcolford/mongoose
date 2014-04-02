man_MANS = compiler.1
EXTRA_DIST = $(man_MANS)
DISTCLEANFILES = $(man_MANS)

compiler.1: $(top_srcdir)/configure.ac $(top_srcdir)/src/compiler.c
	$(AM_V_GEN)$(HELP2MAN) -o $@ $(top_builddir)/src/compiler
