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

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_options
export LSAN_OPTIONS=include_if_exists=${DIR_file}/lsan_options
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/ubsan_options
server_ctrl()
{
   ${DIR_file}/binary/server_ctrl --worldid=0 --start=60 --logpath=/data/log/server_ctrl -d 
}

global_route_1()
{
    ${DIR_file}/binary/global_route_1 --worldid=0 --start=41 --logpath=/data/log/global_route1 -d 
}

global_route_2()
{
    ${DIR_file}/binary/global_route_2 --worldid=0 --start=42 --logpath=/data/log/global_route2 -d 
}

global_route_3()
{
${DIR_file}/binary/global_route_3 --worldid=0 --start=43 --logpath=/data/log/global_route3 -d 
}

global_route_4()
{
${DIR_file}/binary/global_route_4 --worldid=0 --start=44 --logpath=/data/log/global_route4 -d 
}

global_route_5()
{
${DIR_file}/binary/global_route_5 --worldid=0 --start=45 --logpath=/data/log/global_route5 -d 
}

if [ $1 ];
then
    $1;
else
    server_ctrl;
    global_route_1;
    #global_route_2;
    #global_route_3;
    #global_route_4;
    #global_route_5;
fi