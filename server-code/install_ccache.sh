#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

sudo apt-get install ccache
export CCACHE_SLOPPINESS=pch_defines,time_macros
export CCACHE_DIR=/data/mmorpg/server-code/build/.ccache
ccache -M 20G

