#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

ln -sf service_loader global_route_1
ln -sf service_loader global_route_2
ln -sf service_loader global_route_3
ln -sf service_loader global_route_4
ln -sf service_loader global_route_5
ln -sf service_loader server_ctrl

${DIR_file}/server_ctrl --worldid=0 --start=60 -d --logpath=server_ctrl
${DIR_file}/global_route_1 --worldid=0 --start=41 -d --logpath=global_route1
${DIR_file}/global_route_2 --worldid=0 --start=42 -d --logpath=global_route2
${DIR_file}/global_route_3 --worldid=0 --start=43 -d --logpath=global_route3
${DIR_file}/global_route_4 --worldid=0 --start=44 -d --logpath=global_route4
${DIR_file}/global_route_5 --worldid=0 --start=45 -d --logpath=global_route5
