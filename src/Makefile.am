AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_builddir)/lib
AM_CFLAGS = $(WARN_CFLAGS)
AM_YFLAGS = -d
EXTRA_DIST = ast.def ast.tpl

bin_PROGRAMS = compiler

BUILT_SOURCES =					\
ast.c						\
ast.h						\
lex.c						\
lib-recurse					\
parse.c						\
parse.h

compiler_SOURCES =				\
ast.c						\
ast.h						\
ast_util.h					\
collect_vars.c					\
compilation_passes.c				\
compiler.c					\
compiler.h					\
dealias.c					\
gen_code.c					\
lex.l						\
lib.c						\
lib.h						\
loc.h						\
optimizer.c					\
parse.y						\
semantic.c					\
unit.c						\
vars.c

compiler_LDADD = $(top_builddir)/lib/lib$(PACKAGE).la $(LTLIBINTL) $(LIB_ACL)

lib-recurse:
	$(MAKE) -C $(top_builddir)/lib lib$(PACKAGE).la

if RELOCATABLE_VIA_LD
compiler_LDFLAGS = `$(RELOCATABLE_LDFLAGS) $(bindir)`
endif

ast.c ast.h: ast.def ast.tpl
	$(AM_V_GEN)$(AUTOGEN) ast.def

.PHONY: lib-recurse
