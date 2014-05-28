SUBDIRS = lib src man tests po doc
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = m4/gnulib-cache.m4 .version COPYING README.md texi2md/texi2md.pl
BUILT_SOURCES = .version

.version:
	$(AM_V_GEN)echo $(VERSION) > $@

gen-tarball-version:
	$(AM_V_GEN)echo $(VERSION) > $(distdir)/.tarball-version

dist-hook: gen-tarball-version

.PHONY: gen-tarball-version
