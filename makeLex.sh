#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "use: ./<script> xxx.l"
    exit 255
fi
lex $1
if [ "$?" -ne 0 ]; then
    exit $?
fi
gcc -o lexFile lex.yy.c -lfl
if [ "$?" -ne 0 ]; then
    exit $?
fi
exit 0

