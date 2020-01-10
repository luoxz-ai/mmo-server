#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

${DIR_file}/binary/server_ctrl --worldid=0 --stop=60  --logpath=/data/log/server_ctrl
${DIR_file}/binary/global_route_1 --worldid=0 --stop=41  --logpath=/data/log/global_route1
${DIR_file}/binary/global_route_2 --worldid=0 --stop=42  --logpath=/data/log/global_route2
${DIR_file}/binary/global_route_3 --worldid=0 --stop=43  --logpath=/data/log/global_route3
${DIR_file}/binary/global_route_4 --worldid=0 --stop=44  --logpath=/data/log/global_route4
${DIR_file}/binary/global_route_5 --worldid=0 --stop=45  --logpath=/data/log/global_route5
