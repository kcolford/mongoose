man:
	$(MAKE) -C man
	man -l $(top_srcdir)/man/compiler.1

copyright:
	@env UPDATE_COPYRIGHT_HOLDER="Kieran Colford" $(MAKE) update-copyright

.PHONY: copyright man
