#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_options
export LSAN_OPTIONS=include_if_exists=${DIR_file}/lsan_options
server_ctrl()
{
    ${DIR_file}/binary/server_ctrl --worldid=0 --stop=60 --logpath=/data/log/server_ctrl 
}

global_route_1()
{
    ${DIR_file}/binary/global_route_1 --worldid=0 --stop=41 --logpath=/data/log/global_route1 
}

global_route_2()
{
    ${DIR_file}/binary/global_route_2 --worldid=0 --stop=42 --logpath=/data/log/global_route2 
}

global_route_3()
{
${DIR_file}/binary/global_route_3 --worldid=0 --stop=43 --logpath=/data/log/global_route3 
}

global_route_4()
{
${DIR_file}/binary/global_route_4 --worldid=0 --stop=44 --logpath=/data/log/global_route4 
}

global_route_5()
{
${DIR_file}/binary/global_route_5 --worldid=0 --stop=45 --logpath=/data/log/global_route5 
}

if [ $1 ];
then
    $1;
else
    server_ctrl;
    global_route_1;
    global_route_2;
    global_route_3;
    global_route_4;
    global_route_5;   
fi
