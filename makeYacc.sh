#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "use: ./<script> xxx.y"
    exit 255
fi
yacc $1
if [ "$?" -ne 0 ]; then
    exit $?
fi
gcc -o yaccFile y.tab.c -y
if [ "$?" -ne 0 ]; then
    exit $?
fi
exit 0

