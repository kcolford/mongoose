FILES_IN_VC = $(shell ./build-aux/vc-list-files)
GENERATED_FILES_IN_VC = bootstrap COPYING doc.cfg.in fix-cleanup.patch
WRITTEN_FILES = $(filter-out $(GENERATED_FILES_IN_VC), $(FILES_IN_VC))

config_h_header = "config\.h"
update-copyright-env = UPDATE_COPYRIGHT_HOLDER="Kieran Colford"
local-checks-to-skip = sc_trailing_blank
VC_LIST_ALWAYS_EXCLUDE_REGEX = 'tests/.*\.c$$'

man: $(srcdir)/.version
	$(MAKE) -C man
	man -l $(top_srcdir)/man/compiler.1
.PHONY: man

count:
	@echo 'Line Counts'
	@wc -l $(WRITTEN_FILES)
.PHONY: count

suite:
	@for i in $(top_builddir)/tests/*; do	\
	  if [ -f $$i/test-suite.log ]; then	\
	    cat $$i/test-suite.log;		\
	  fi;					\
	done 
.PHONY: suite
