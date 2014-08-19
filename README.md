Compiler
========

[![Build Status]
(https://travis-ci.org/kcolford/compiler.svg?branch=master)]
(https://travis-ci.org/kcolford/compiler)

An experimental C compiler.

This is a project that attempts to become a fully fledged C compiler.
It is currently Turing Complete and can be linked in to any other
library.

It currently has no support for C's type system, all types are just
64-bit numbers.  It has no structs, unions, or enums.  All pointers
are the same as int.  Because of the lack of a type system but the
need to know when to allocate a variable, a type must be given to
declare a variable but that type is ignored.

You can clone the git repository with the following command:

    git clone https://github.com/kcolford/compiler.git

Access to a distribution tar ball is currently only available through
someone who has already downloaded a clone of the repository.

License
-------

Copyright (C) 2014 Kieran Colford

Permission is granted to copy, distribute and/or modify this document
under the terms of the GNU Free Documentation License, Version 1.3 or
any later version published by the Free Software Foundation; with no
Invariant Sections, no Front-Cover Texts, and no Back-Cover Texts.  A
copy of the license is included in the section entitled "GNU Free
Documentation License".

Prerequisites
-------------

In order to use this project you need the following list of installed
programs,

- Autoconf*
- Autoconf Archive*
- AutoGen*
- Automake*
- Bash
- Bison*
- Doxygen+
- Flex*
- GCC
- GIT*
- Help2man*+
- Make
- Perl+
- Tar
- Valgrind (Optional)

*: Only required if you're a developer or acquired the source from a
 source repository.

+: Only required for generating documentation.

How to Set Up
-------------

The first step to working on or using Compiler is if you retrieved the
sources from a checkout repository you have to run the bootstrap
script like this,

    ./bootstrap

With this done, you can now run the configure script and make,

    ./configure && make

For more information, read the INSTALL file distributed with this
package.

What To Do
----------

If you don't know how Compiler works, read through the documentation.
It has all been generated with Doxygen and thus should be easy to
generate read through.  To generate the documentation, just run:

    make html

If you can't think of something to work on, see the Doxygen generated
Todo List.

Alternatively, you can work on Compiler's general documentation in the
README.md.

A Note on Extra Documentation
-----------------------------

Additional documentation was provided by gnulib in TeXinfo format.
This is temporarily translated into markdown by the Perl script
texi2md.pl and then fed into Doxygen's native markdown processor.

The texi2md.pl script is distributed along with the source code for
Compiler and can be found in a git repository at
<https://github.com/kcolford/texi2md>.  It also only alters the
TeXinfo markup (translating it to markdown).  Thus it satisfies all
requirements of verbatim copying as described by the GNU Free
Documentation License.
