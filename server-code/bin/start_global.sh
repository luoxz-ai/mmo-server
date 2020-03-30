#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

mkdir -p binary
mkdir -p /data/log
cd binary
ln -sf ../service_loader global_all
cd -

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_options
export LSAN_OPTIONS=include_if_exists=${DIR_file}/lsan_options
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/ubsan_options

${DIR_file}/binary/global_all --worldid=0 --start --logpath=/data/log/global_all -d
