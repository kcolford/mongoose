man:
	$(MAKE) -C man
	man -l $(top_srcdir)/man/compiler.1
.PHONY: man
