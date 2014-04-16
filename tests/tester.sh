
prog=`mktemp`

if [ x"$COMPILER" = x ]; then
    exit 77
fi

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
