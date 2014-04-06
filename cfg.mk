FILES_IN_VC = $(shell ./build-aux/vc-list-files)
GENERATED_FILES_IN_VC = bootstrap COPYING
WRITTEN_FILES = $(filter-out $(GENERATED_FILES_IN_VC) cfg.mk, $(FILES_IN_VC))

man: $(srcdir)/src/compiler.c $(srcdir)/.version
	$(MAKE) -C man
	man -l $(top_srcdir)/man/compiler.1

copyright:
	@env UPDATE_COPYRIGHT_HOLDER="Kieran Colford" $(MAKE) update-copyright

todo-list:
	@grep 'TODO' $(WRITTEN_FILES) | sed -e 's/:[ \t]*/: /'
	@grep 'FIXME' $(WRITTEN_FILES) | sed -e 's/:[ \t]*/: /'
	@grep 'XXX' $(WRITTEN_FILES) | sed -e 's/:[ \t]*/: /'

count:
	@echo
	@echo 'Line Counts'
	@wc -l $(WRITTEN_FILES) cfg.mk

.PHONY: copyright count man todo-list
