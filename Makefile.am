SUBDIRS = lib src man tests po
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = m4/gnulib-cache.m4 .version COPYING README.md doc.cfg.in
BUILT_SOURCES = .version

html: doc.cfg
	$(AM_V_GEN)doxygen $<

doc: html

distclean-local:
	$(RM) -r $(top_builddir)/doc/html

.version:
	$(AM_V_GEN)echo $(VERSION) > $@

gen-tarball-version:
	$(AM_V_GEN)echo $(VERSION) > $(distdir)/.tarball-version

dist-hook: gen-tarball-version

.PHONY: doc gen-tarball-version
