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
ln -sf ../service_loader z${serverid}_world
ln -sf ../service_loader z${serverid}_zone1
ln -sf ../service_loader z${serverid}_zone2
ln -sf ../service_loader z${serverid}_zone3
ln -sf ../service_loader z${serverid}_zone4
ln -sf ../service_loader z${serverid}_zone5
ln -sf ../service_loader z${serverid}_socket1
ln -sf ../service_loader z${serverid}_socket2
ln -sf ../service_loader z${serverid}_socket3
ln -sf ../service_loader z${serverid}_socket4
ln -sf ../service_loader z${serverid}_socket5
cd -

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_options
export LSAN_OPTIONS=include_if_exists=${DIR_file}/lsan_options

${DIR_file}/binary/z${serverid}_world --worldid=${serverid} --start=1,3,4 --logpath=/data/log/zone_${serverid}/world -d
${DIR_file}/binary/z${serverid}_zone1 --worldid=${serverid} --start=11,21 --logpath=/data/log/zone_${serverid}/zone1 -d
${DIR_file}/binary/z${serverid}_zone2 --worldid=${serverid} --start=12,22 --logpath=/data/log/zone_${serverid}/zone2 -d
${DIR_file}/binary/z${serverid}_zone3 --worldid=${serverid} --start=13,23 --logpath=/data/log/zone_${serverid}/zone3 -d
${DIR_file}/binary/z${serverid}_zone4 --worldid=${serverid} --start=14,24 --logpath=/data/log/zone_${serverid}/zone4 -d
${DIR_file}/binary/z${serverid}_zone5 --worldid=${serverid} --start=15,25 --logpath=/data/log/zone_${serverid}/zone5 -d
${DIR_file}/binary/z${serverid}_socket1 --worldid=${serverid} --start=31 --logpath=/data/log/zone_${serverid}/socket1 -d
${DIR_file}/binary/z${serverid}_socket2 --worldid=${serverid} --start=32 --logpath=/data/log/zone_${serverid}/socket2 -d
${DIR_file}/binary/z${serverid}_socket3 --worldid=${serverid} --start=33 --logpath=/data/log/zone_${serverid}/socket3 -d
${DIR_file}/binary/z${serverid}_socket4 --worldid=${serverid} --start=34 --logpath=/data/log/zone_${serverid}/socket4 -d
${DIR_file}/binary/z${serverid}_socket5 --worldid=${serverid} --start=35 --logpath=/data/log/zone_${serverid}/socket5 -d
