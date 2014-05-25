FILES_IN_VC = $(shell ./build-aux/vc-list-files)
GENERATED_FILES_IN_VC = bootstrap COPYING doc.cfg.in fix-cleanup.patch
WRITTEN_FILES = $(filter-out $(GENERATED_FILES_IN_VC) cfg.mk, $(FILES_IN_VC))

COMPILER_OPTS =
export COMPILER_OPTS

local-checks-to-skip = sc_trailing_blank sc_vulnerable_makefile_CVE-2012-3386
config_h_header = "config\.h"

man: $(srcdir)/.version
	$(MAKE) -C man
	man -l $(top_srcdir)/man/compiler.1

copyright:
	@env UPDATE_COPYRIGHT_HOLDER="Kieran Colford" $(MAKE) update-copyright

fix-files:
	@sed -i -e '/^ \* @date/d' $(WRITTEN_FILES)

count:
	@echo
	@echo 'Line Counts'
	@wc -l $(WRITTEN_FILES) cfg.mk

.PHONY: copyright count fix-files man
