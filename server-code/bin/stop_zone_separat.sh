#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`



serverid=$1
if [ ! -n "$1" ] ;then  
    serverid=1001
fi 


export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_options
export LSAN_OPTIONS=include_if_exists=${DIR_file}/lsan_options
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/ubsan_options

${DIR_file}/binary/z${serverid}_world --worldid=${serverid} --stop=1,3,4 --logpath=/data/log/zone_${serverid}/world
${DIR_file}/binary/z${serverid}_zone1 --worldid=${serverid} --stop=11,21 --logpath=/data/log/zone_${serverid}/zone1
${DIR_file}/binary/z${serverid}_zone2 --worldid=${serverid} --stop=12,22 --logpath=/data/log/zone_${serverid}/zone2
${DIR_file}/binary/z${serverid}_zone3 --worldid=${serverid} --stop=13,23 --logpath=/data/log/zone_${serverid}/zone3
${DIR_file}/binary/z${serverid}_zone4 --worldid=${serverid} --stop=14,24 --logpath=/data/log/zone_${serverid}/zone4
${DIR_file}/binary/z${serverid}_zone5 --worldid=${serverid} --stop=15,25 --logpath=/data/log/zone_${serverid}/zone5
${DIR_file}/binary/z${serverid}_socket1 --worldid=${serverid} --stop=31 --logpath=/data/log/zone_${serverid}/socket1
${DIR_file}/binary/z${serverid}_socket2 --worldid=${serverid} --stop=32 --logpath=/data/log/zone_${serverid}/socket2
${DIR_file}/binary/z${serverid}_socket3 --worldid=${serverid} --stop=33 --logpath=/data/log/zone_${serverid}/socket3
${DIR_file}/binary/z${serverid}_socket4 --worldid=${serverid} --stop=34 --logpath=/data/log/zone_${serverid}/socket4
${DIR_file}/binary/z${serverid}_socket5 --worldid=${serverid} --stop=35 --logpath=/data/log/zone_${serverid}/socket5
