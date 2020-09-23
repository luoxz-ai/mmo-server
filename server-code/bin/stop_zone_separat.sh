#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`



serverid=$1
if [ ! -n "$1" ] ;then  
    serverid=1
fi 


export ASAN_OPTIONS=include_if_exists=${DIR_file}/options_asan
export LSAN_OPTIONS=include_if_exists=${DIR_file}/options_lsan
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/options_ubsan
export TSAN_OPTIONS=include_if_exists=${DIR_file}/options_tsan

${DIR_file}/binary/z${serverid}_world --worldid=${serverid} --stop=WORLD_SERVICE-0,MARKET_SERVICE-0,GM_SERVICE-0,ROUTE_SERVICE-0 --logpath=/data/log/zone_${serverid}/world 
${DIR_file}/binary/z${serverid}_scene1 --worldid=${serverid} --stop=SCENE_SERVICE-1,AI_SERVICE-1 --logpath=/data/log/zone_${serverid}/zone1 
${DIR_file}/binary/z${serverid}_scene2 --worldid=${serverid} --stop=SCENE_SERVICE-2,AI_SERVICE-2 --logpath=/data/log/zone_${serverid}/zone2 
${DIR_file}/binary/z${serverid}_scene3 --worldid=${serverid} --stop=SCENE_SERVICE-3,AI_SERVICE-3 --logpath=/data/log/zone_${serverid}/zone3 
${DIR_file}/binary/z${serverid}_scene4 --worldid=${serverid} --stop=SCENE_SERVICE-4,AI_SERVICE-4 --logpath=/data/log/zone_${serverid}/zone4 
${DIR_file}/binary/z${serverid}_scene5 --worldid=${serverid} --stop=SCENE_SERVICE-5,AI_SERVICE-5 --logpath=/data/log/zone_${serverid}/zone5 
${DIR_file}/binary/z${serverid}_socket1 --worldid=${serverid} --stop=SOCKET_SERVICE-1,AUTH_SERVICE-1 --logpath=/data/log/zone_${serverid}/socket1 
${DIR_file}/binary/z${serverid}_socket2 --worldid=${serverid} --stop=SOCKET_SERVICE-2,AUTH_SERVICE-2 --logpath=/data/log/zone_${serverid}/socket2 
${DIR_file}/binary/z${serverid}_socket3 --worldid=${serverid} --stop=SOCKET_SERVICE-3,AUTH_SERVICE-3 --logpath=/data/log/zone_${serverid}/socket3 
${DIR_file}/binary/z${serverid}_socket4 --worldid=${serverid} --stop=SOCKET_SERVICE-4,AUTH_SERVICE-4 --logpath=/data/log/zone_${serverid}/socket4 
${DIR_file}/binary/z${serverid}_socket5 --worldid=${serverid} --stop=SOCKET_SERVICE-5,AUTH_SERVICE-5 --logpath=/data/log/zone_${serverid}/socket5 
