EXTRA_DIST = fdl.texi gpl-3.0.texi relocatable.texi

pdf-am: doc
	[ "$(GENERATE_LATEX)" = "YES" ] && $(MAKE) -C latex

html-am: doc

doc: doc.cfg
	$(AM_V_GEN)$(DOXYGEN) $<

clean-local:
	$(RM) -r html latex

.PHONY: doc
