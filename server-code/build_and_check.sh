#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

mkdir -p build
cd build/
set -e
CC=gcc-7 CXX=g++-7 cmake .. -DDEBUG=ON -DTIDY=ON
make -j4
