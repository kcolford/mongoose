AM_CPPFLAGS = -I$(top_srcdir)/lib
AM_YFLAGS = -d
EXTRA_DIST = ast.def ast.tpl

bin_PROGRAMS = compiler

BUILT_SOURCES =					\
ast.c						\
ast.h						\
lex.c						\
parse.c						\
parse.h

compiler_SOURCES =				\
ast.c						\
ast.h						\
compiler.c					\
compiler.h					\
gen_code.c					\
lex.l						\
lib.c						\
optimizer.c					\
parse.y						\
semantic.c

compiler_LDADD = $(top_builddir)/lib/libcompiler.la

ast.c: ast.def ast.tpl
	$(AM_V_GEN)$(AUTOGEN) ast.def

ast.h: ast.c
ast.o: parse.h
