#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

${DIR_file}/server_ctrl --worldid=0 --stop=60  --logpath=server_ctrl
${DIR_file}/global_route_1 --worldid=0 --stop=41  --logpath=global_route1
${DIR_file}/global_route_2 --worldid=0 --stop=42  --logpath=global_route2
${DIR_file}/global_route_3 --worldid=0 --stop=43  --logpath=global_route3
${DIR_file}/global_route_4 --worldid=0 --stop=44  --logpath=global_route4
${DIR_file}/global_route_5 --worldid=0 --stop=45  --logpath=global_route5
