#ifndef COMPILER_H
#define COMPILER_H

#include "ast.h"
#include "parse.h"
#include <stdio.h>
#include <stddef.h>

extern FILE *yyin;
extern FILE *outfile;
extern int optimize;
extern int debug;

extern int gen_code (struct ast *);
extern int optimizer (struct ast **);
extern int semantic (struct ast *);

extern char *my_strcat (char *, char *);
extern char *place_holder (void);
extern char *my_printf (const char *, ...);
extern void *xmalloc (size_t);
extern void xfree (void *);
extern char *xstrdup (const char *);
extern void *xmemdup (const void *, size_t);
extern char *tmpfile_name (void);

#endif
