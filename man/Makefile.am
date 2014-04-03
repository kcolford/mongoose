man_MANS = compiler.1
EXTRA_DIST = $(man_MANS)
DISTCLEANFILES = $(man_MANS)

compiler.1: $(top_srcdir)/src/compiler.c $(srcdir)/Makefile.am
	$(AM_V_GEN)$(HELP2MAN) -N -o $@ $(top_builddir)/src/compiler
