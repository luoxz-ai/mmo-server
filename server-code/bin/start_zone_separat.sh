#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

serverid=$1
if [ ! -n "$1" ] ;then  
    serverid=1001
fi 


ln -sf service_loader z${serverid}_world
ln -sf service_loader z${serverid}_zone1
ln -sf service_loader z${serverid}_zone2
ln -sf service_loader z${serverid}_zone3
ln -sf service_loader z${serverid}_zone4
ln -sf service_loader z${serverid}_zone5
ln -sf service_loader z${serverid}_socket1
ln -sf service_loader z${serverid}_socket2
ln -sf service_loader z${serverid}_socket3
ln -sf service_loader z${serverid}_socket4
ln -sf service_loader z${serverid}_socket5

${DIR_file}/z${serverid}_world --worldid=${serverid} --start=1,3,4 -d --logpath=zone_${serverid}/world
${DIR_file}/z${serverid}_zone1 --worldid=${serverid} --start=11,21 -d --logpath=zone_${serverid}/zone1
${DIR_file}/z${serverid}_zone2 --worldid=${serverid} --start=12,22 -d --logpath=zone_${serverid}/zone2
${DIR_file}/z${serverid}_zone3 --worldid=${serverid} --start=13,23 -d --logpath=zone_${serverid}/zone3
${DIR_file}/z${serverid}_zone4 --worldid=${serverid} --start=14,24 -d --logpath=zone_${serverid}/zone4
${DIR_file}/z${serverid}_zone5 --worldid=${serverid} --start=15,25 -d --logpath=zone_${serverid}/zone5
${DIR_file}/z${serverid}_socket1 --worldid=${serverid} --start=31 -d --logpath=zone_${serverid}/socket1
${DIR_file}/z${serverid}_socket2 --worldid=${serverid} --start=32 -d --logpath=zone_${serverid}/socket2
${DIR_file}/z${serverid}_socket3 --worldid=${serverid} --start=33 -d --logpath=zone_${serverid}/socket3
${DIR_file}/z${serverid}_socket4 --worldid=${serverid} --start=34 -d --logpath=zone_${serverid}/socket4
${DIR_file}/z${serverid}_socket5 --worldid=${serverid} --start=35 -d --logpath=zone_${serverid}/socket5
