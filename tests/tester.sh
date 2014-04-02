#!/bin/sh

srcfile=$1
prog=`mktemp`

compiler -o $prog $srcfile || exit 1
chmod u+x $prog
myout=`mktemp`
$prog > $myout || { rm $myout $prog; exit 1; }

nativeout=`mktemp`
$CC -o $prog $srcfile 2> /dev/null
$prog > $nativeout

if cmp -s $myout $nativeout; then
    rm $myout $nativeout $prog
    exit 0
else
    rm $myout $nativeout $prog
    exit 1
fi
