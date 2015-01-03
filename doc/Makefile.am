EXTRA_DIST = fdl.texi gpl-3.0.texi

if HAVE_DOXYGEN
html/index.html: doc.cfg
	$(AM_V_GEN)$(DOXYGEN) $<

html-local: html/index.html

if GENERATE_LATEX
pdf-local: pdf/refman.tex
	$(MAKE) -C latex pdf

pdf/refman.tex: html/index.html
	$(AM_V_at)test -f $@ || { rm -f html/index.html; \
	  $(MAKE) html/index.html; }
endif

endif

clean-local:
	$(RM) -r html latex
