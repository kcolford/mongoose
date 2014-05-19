SUBDIRS = lib src man tests po
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = m4/gnulib-cache.m4 .version COPYING README doc.cfg.in
BUILT_SOURCES = .version

html: doc.cfg
	$(DOXYGEN) $<
.PHONY: html

.version:
	$(AM_V_GEN)echo $(VERSION) > $@

gen-tarball-version:
	$(AM_V_GEN)echo $(VERSION) > $(distdir)/.tarball-version
.PHONY: gen-tarball-version

dist-hook: gen-tarball-version
