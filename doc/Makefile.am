EXTRA_DIST = fdl.texi gpl-3.0.texi

if GENERATE_LATEX
pdf-local: doc
	$(MAKE) -C latex
endif

html-local: doc

doc: doc.cfg
	$(AM_V_GEN)$(DOXYGEN) $<

clean-local:
	$(RM) -r html latex

.PHONY: doc
