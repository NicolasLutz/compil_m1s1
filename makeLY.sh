#!/bin/bash
if [ "$#" -ne 2 ]; then
    echo "use: ./<script> xxx.y xxx.l"
    exit 255
fi
yacc -d $1
if [ "$?" -ne 0 ]; then
    exit $?
fi
lex $2
if [ "$?" -ne 0 ]; then
    exit $?
fi
gcc -o lyFile lex.yy.c y.tab.c -lfl
if [ "$?" -ne 0 ]; then
    exit $?
fi
exit 0

