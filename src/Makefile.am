AM_CPPFLAGS = -I$(top_srcdir)/lib \
              -I$(top_builddir)/lib \
              -DARCHDIR=\"$(archdir)\"
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
extendf.h					\
free.h						\
gen_code.c					\
lex.l						\
lib.c						\
lib.h						\
loc.c						\
loc.h						\
my_printf.c					\
optimizer.c					\
parse.y						\
place_holder.h					\
safe_system.c					\
safe_system.h					\
semantic.c					\
tmpfile_name.h					\
transform.c					\
unit.c						\
vars.c

compiler_LDADD = $(top_builddir)/lib/lib$(PACKAGE).la $(LTLIBINTL) $(LIB_ACL)

lib-recurse:
	$(MAKE) -C $(top_builddir)/lib lib$(PACKAGE).la

ast.c: ast.def ast.tpl
	$(AM_V_GEN)$(AUTOGEN) ast.def
ast.h: ast.c
	$(AM_V_at)test -f $@ || { rm -f ast.c; $(MAKE) ast.c; }

.PHONY: lib-recurse
