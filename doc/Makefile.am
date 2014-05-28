EXTRA_DIST = fdl.texi gpl-3.0.texi relocatable.texi

pdf-am: doc
	@if [ "$(GENERATE_LATEX)" = "YES" ]; then \
	  $(MAKE) -C latex; \
	fi

html-am: doc

doc: $(srcdir)/doc.cfg
	$(AM_V_GEN)$(DOXYGEN) $<

clean-local:
	$(RM) -r html latex

.PHONY: doc
