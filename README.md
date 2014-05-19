Compiler Developer's Manual
===========================

Prerequisites
-------------

In order to use this project you need the following list of installed
programs,

- Autoconf*
- AutoGen*
- Automake*
- Bash
- Bison*
- Doxygen
- GCC
- GIT*
- Help2man*
- Lex*
- Make
- Tar

*: Only required if you're a developer.

How to Set Up
-------------

The first step to working on or using Compiler is if you retrieved the
sources from a checkout repository you have to run the bootstrap
script like this,

        $ ./bootstrap

With this done, you can now run the configure script and make,

        $ ./configure && make

What To Do
----------

If you don't know how Compiler works, read through the documentation.
It has all been generated with Doxygen and thus should be easy to
generate read through.  To generate the documentation, just run:

	$ make doc

If you can't think of something to work on, see the Doxygen generated
Todo List.

Alternatively, you can work on Compiler's general documentation in the
README.md.

Translators
-----------

TODO: Instructions for translators.
