SUBDIRS = lib src man tests po
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = m4/gnulib-cache.m4 .version COPYING README.md doc.cfg.in
BUILT_SOURCES = .version

doc: doc.cfg
	$(AM_V_GEN)$(DOXYGEN) $<

html: doc

pdf: doc
	@if [ "@GENERATE_LATEX@" = "YES" ]; then \
	  $(MAKE) -C doc/latex refman.pdf; \
	fi

distclean-local:
	$(RM) -r $(builddir)/doc/html $(builddir)/doc/latex

.version:
	$(AM_V_GEN)echo $(VERSION) > $@

gen-tarball-version:
	$(AM_V_GEN)echo $(VERSION) > $(distdir)/.tarball-version

dist-hook: gen-tarball-version

.PHONY: doc gen-tarball-version pdf-make
