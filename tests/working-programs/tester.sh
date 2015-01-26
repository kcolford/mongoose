#!/bin/sh

srcfile=$1

tmpdir=`mktemp --directory mongoose-tester.XXXXXXXX`

prog=$tmpdir/prog; touch $prog
myout=$tmpdir/myout; touch $myout
nativeout=$tmpdir/nativeout; touch $nativeout

die () {
    [ "$1" = 0 ] || echo "FAILED: $2" >&2
    rm -f $prog $myout $nativeout
    rmdir $tmpdir
    exit $1
}

run () {
    msg=$1
    shift
    if "$@"; then
	:
    else
	die $? $msg
    fi
}

run "the regular C compiler failed" $CC -DGCC -o $prog $srcfile -lm

run "the regular C compiler could not create an executable" [ -x $prog ] && \
    run "the regular C compiler's executable failed" $prog > $nativeout

mycompile () {    
    run "could not compile $srcfile with options: $@" \
	$COMPILER $@ -o $prog $srcfile

    run "the program is not runable with options: $@" [ -x $prog ] && \
	run "the program failed to run with options: $@" $prog > $myout

    run "different output with options: $@" \
	cmp $myout $nativeout
}

mycompile
mycompile -O
die 0
