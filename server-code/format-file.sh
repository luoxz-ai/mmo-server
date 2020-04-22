#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

cd src
find . -name "*.cpp" ! -path "*/proto/*" -exec clang-format-9 -i {} \; 
find . -name "*.h" ! -path "*/proto/*" -exec clang-format-9 -i {} \;
find . -name "*.proto" -exec clang-format-9 -i {} \;
cd -
