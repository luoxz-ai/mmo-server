#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

mkdir -p binary
mkdir -p /data/log
cd binary
ln -sf ../service_loader shared_zone_1
ln -sf ../service_loader shared_zone_2
ln -sf ../service_loader shared_zone_3
ln -sf ../service_loader shared_zone_4
ln -sf ../service_loader shared_zone_5
cd -

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_options
export LSAN_OPTIONS=include_if_exists=${DIR_file}/lsan_options
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/ubsan_options

${DIR_file}/binary/shared_zone_1 --worldid=0 --start=16,26 --logpath=/data/log/shared_zone_1 -d 
${DIR_file}/binary/shared_zone_2 --worldid=0 --start=17,27 --logpath=/data/log/shared_zone_2 -d 
${DIR_file}/binary/shared_zone_3 --worldid=0 --start=18,28 --logpath=/data/log/shared_zone_3 -d 
${DIR_file}/binary/shared_zone_4 --worldid=0 --start=19,29 --logpath=/data/log/shared_zone_4 -d 
${DIR_file}/binary/shared_zone_5 --worldid=0 --start=20,30 --logpath=/data/log/shared_zone_5 -d 