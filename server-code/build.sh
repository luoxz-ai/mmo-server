#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

mkdir -p build
cd build/
set -e
CC=clang-9 CXX=clang++-9 cmake .. -DDEBUG=ON
#CC=gcc-7 CXX=g++-7 cmake .. -DDEBUG=ON
make -j3
