#!/bin/sh

ret=0
srcfile=$1
prog=`mktemp`

compiler -o $prog $srcfile || ret=1
myout=`mktemp`
$prog > $myout || ret=1

nativeout=`mktemp`
$CC -o $prog $srcfile 2> /dev/null
$prog > $nativeout

cmp -s $myout $nativeout || ret=1

rm $myout $nativeout $prog
exit $ret
