#!/bin/bash

cd ..
echo "rebuilding binary..."
make clean && make
cd test

PROG=../bin/raycast

for f in *.json;
do
    echo "testing $f"
    ${PROG} $f
done
