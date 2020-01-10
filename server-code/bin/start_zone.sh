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

${DIR_file}/binary/zone_${serverid} --worldid=$serverid --start -d --logpath=/data/log/zone_$serverid
