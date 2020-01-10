#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

mkdir -p binary
mkdir -p /data/log
cd binary

ln -sf ../service_loader global_route_1
ln -sf ../service_loader global_route_2
ln -sf ../service_loader global_route_3
ln -sf ../service_loader global_route_4
ln -sf ../service_loader global_route_5
ln -sf ../service_loader server_ctrl
cd -

${DIR_file}/binary/server_ctrl --worldid=0 --start=60 -d --logpath=/data/log/server_ctrl
${DIR_file}/binary/global_route_1 --worldid=0 --start=41 -d --logpath=/data/log/global_route1
${DIR_file}/binary/global_route_2 --worldid=0 --start=42 -d --logpath=/data/log/global_route2
${DIR_file}/binary/global_route_3 --worldid=0 --start=43 -d --logpath=/data/log/global_route3
${DIR_file}/binary/global_route_4 --worldid=0 --start=44 -d --logpath=/data/log/global_route4
${DIR_file}/binary/global_route_5 --worldid=0 --start=45 -d --logpath=/data/log/global_route5
