man_MANS = mongoose.1
EXTRA_DIST = $(man_MANS)
DISTCLEANFILES = $(man_MANS)

mongoose.1: $(top_srcdir)/src/compiler.c $(top_srcdir)/.version
	$(MAKE) -C $(top_builddir)/src
	$(AM_V_GEN) \
	if [ "$(HELP2MAN)" ]; then \
	  $(HELP2MAN) -N -o $@ $(top_builddir)/src/mongoose; \
	fi

$(top_srcdir)/.version:
	$(MAKE) -C $(top_srcdir) $@
