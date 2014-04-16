#!/bin/sh

ret=0
srcfile=$1
prog=`mktemp`

$COMPILER -o $prog $srcfile || ret=1
myout=`mktemp`
if [ -x $prog ]; then
    $prog > $myout || ret=1
else
    ret=1
fi

nativeout=`mktemp`
$CC -o $prog $srcfile 2> /dev/null
if [ -x $prog ]; then
    $prog > $nativeout || ret=1
else
    ret=1
fi

cmp -s $myout $nativeout || ret=1

rm $myout $nativeout $prog
exit $ret
