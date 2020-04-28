#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

cd $1
pwd
find . -name "*.cpp" ! -path "*/proto/*" -exec clang-format-9 -i --verbose {} \; 
find . -name "*.h" ! -path "*/proto/*" -exec clang-format-9 -i --verbose {} \;
find . -name "*.proto" -exec clang-format-9 -i --verbose {}\;
cd -
