#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

mkdir -p build
cd build/
set -e

use_make()
{
    CCACHE_SLOPPINESS=pch_defines,time_macros CC=clang-9 CXX=clang++-9 cmake -DDEBUG=OFF -DTIDY=OFF .. 
    make -j3
}

use_ninja()
{
    CCACHE_SLOPPINESS=pch_defines,time_macros CC=clang-9 CXX=clang++-9 cmake -DDEBUG=OFF -DTIDY=OFF -GNinja .. 
    NINJA_STATUS="[%p/%f/%t %e] " ninja
}

if [ $1 ];
then
    $1;
else
    use_ninja;
fi
