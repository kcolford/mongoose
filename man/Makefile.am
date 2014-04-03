man_MANS = compiler.1
EXTRA_DIST = $(man_MANS)
DISTCLEANFILES = $(man_MANS)

compiler.1: $(top_srcdir)/src/compiler.c $(top_srcdir)/.version
	$(MAKE) -C $(top_builddir)/src
	$(AM_V_GEN)$(HELP2MAN) -N -o $@ $(top_builddir)/src/compiler
