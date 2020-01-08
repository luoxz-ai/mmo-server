#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`



serverid=$1
if [ ! -n "$1" ] ;then  
    serverid=1001
fi 


ln -sf service_loader zone_${serverid}

${DIR_file}/zone_${serverid} --worldid=$serverid --stop --logpath=zone_$serverid
