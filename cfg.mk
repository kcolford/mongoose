FILES_IN_VC = $(shell ./build-aux/vc-list-files)
GENERATED_FILES_IN_VC = bootstrap COPYING doc.cfg.in fix-cleanup.patch
WRITTEN_FILES = $(filter-out $(GENERATED_FILES_IN_VC) cfg.mk, $(FILES_IN_VC))

COMPILER_OPTS =
export COMPILER_OPTS

config_h_header = "config\.h"
update-copyright-env = UPDATE_COPYRIGHT_HOLDER="Kieran Colford"
local-checks-to-skip = sc_trailing_blank sc_vulnerable_makefile_CVE-2012-3386
VC_LIST_ALWAYS_EXCLUDE_REGEX = 'tests/.*\.c$$'

man: $(srcdir)/.version
	$(MAKE) -C man
	man -l $(top_srcdir)/man/compiler.1

count:
	@echo 'Line Counts'
	@wc -l $(WRITTEN_FILES) cfg.mk

.PHONY: count man
