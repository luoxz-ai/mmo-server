#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

serverid=$1
if [ ! -n "$1" ] ;then  
    serverid=1001
fi 

mkdir -p binary
mkdir -p /data/log
cd binary
ln -sf ../service_loader zone_${serverid}
cd -

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_options
export LSAN_OPTIONS=include_if_exists=${DIR_file}/lsan_options
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/ubsan_options

${DIR_file}/binary/zone_${serverid} --worldid=$serverid --start --logpath=/data/log/zone_$serverid -d
