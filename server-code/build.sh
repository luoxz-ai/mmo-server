#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

mkdir -p build
cd build/
set -e
CCACHE_SLOPPINESS=pch_defines,time_macros CC=clang-9 CXX=clang++-9 cmake .. -DDEBUG=ON -DUSE_ASAN=ON -DUSE_UBSAN=ON
#CC=gcc-7 CXX=g++-7 cmake .. -DDEBUG=ON
make -j3
