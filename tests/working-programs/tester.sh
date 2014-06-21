#!/bin/sh

ret=0
srcfile=$1

prog=`mktemp`

if [ x"$COMPILER" = x ]; then
    exit 77
fi

logname=$srcfile.log

$COMPILER -o $prog $srcfile | tee $logname || ret=1
if [ -s $logname ]; then
    ret=1
else
    rm -f $logname
fi

myout=`mktemp`
if [ -x $prog ]; then
    $prog > $myout || ret=1
else
    ret=1
fi

nativeout=`mktemp`
$CC -DGCC -o $prog $srcfile -lm 2> /dev/null || ret=1
if [ -x $prog ]; then
    $prog > $nativeout || ret=1
else
    ret=1
    echo "The regular C compiler failed..." >&2
fi

cmp -s $myout $nativeout || ret=1

rm $myout $nativeout $prog

exit $ret
