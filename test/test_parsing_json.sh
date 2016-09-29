#!/bin/bash

cd ..
echo "rebuilding binary..."
make clean && make
cd test

PROG=../bin/raycast
i=0
for f in parsing_tests/*.json;
do
    echo "testing $f"
    ${PROG} $f
    i=$(($i+1))
done
echo "finished testing $i files"
