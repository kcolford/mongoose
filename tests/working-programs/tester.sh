#!/bin/sh

srcfile=$1

prog=`mktemp`
myout=`mktemp`
nativeout=`mktemp`

run () {
    msg=$1
    shift
    if "$@"; then
	:
    else
	code=$?
	echo "FAILED: $msg" >&2
	rm -f $prog $myout $nativeout
	exit $code
    fi
}

run "could not compile $srcfile" $COMPILER -o $prog $srcfile

run "the program is not runable" [ -x $prog ] && \
    run "the program failed to run" $prog > $myout

run "the regular C compiler failed" $CC -DGCC -o $prog $srcfile -lm

run "the regular C compiler could not create an executable" [ -x $prog ] && \
    run "the regular C compiler's executable failed" $prog > $nativeout

run "the two compilers created programs with different output" \
    cmp $myout $nativeout
