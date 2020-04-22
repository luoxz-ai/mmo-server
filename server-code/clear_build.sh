#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

cd build/
rm CMakeFiles -rf
rm obj -rf
rm Makefile
rm CMakeCache.txt
rm cmake_install.cmake