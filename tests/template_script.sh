#!/bin/sh

srcfile=`mktemp --suffix=.c`
cat - > $srcfile <<EOF

EOF

ret=0
./tester.sh $srcfile || ret=1
rm $srcfile
exit $ret
