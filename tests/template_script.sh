#!/bin/sh

srcfile=file.c
cat - > $srcfile <<EOF

EOF

ret=0
./tester.sh $srcfile $0 || ret=1
rm $srcfile
exit $ret
