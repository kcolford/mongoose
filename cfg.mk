FILES_IN_VC = $(shell ./build-aux/vc-list-files)
GENERATED_FILES_IN_VC = bootstrap COPYING doc.cfg.in
WRITTEN_FILES = $(filter-out $(GENERATED_FILES_IN_VC) cfg.mk, $(FILES_IN_VC))

COMPILER_ENV =
export COMPILER_ENV
COMPILER_OPTS =
export COMPILER_OPTS

local-checks-to-skip = sc_prohibit_strcmp
config_h_header = "config\.h"

man: $(srcdir)/src/compiler.c $(srcdir)/.version
	$(MAKE) -C man
	man -l $(top_srcdir)/man/compiler.1

copyright:
	@env UPDATE_COPYRIGHT_HOLDER="Kieran Colford" $(MAKE) update-copyright

fix-files:
	@sed -i -e '/^ \* @date/d' $(WRITTEN_FILES)
	@sed -i -e 's/[ \t]*$$//' $(WRITTEN_FILES)

count:
	@echo
	@echo 'Line Counts'
	@wc -l $(WRITTEN_FILES) cfg.mk

.PHONY: copyright count fix-files man
