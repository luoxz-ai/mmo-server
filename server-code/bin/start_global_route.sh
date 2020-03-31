#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

mkdir -p binary
mkdir -p /data/log
cd binary

ln -sf ../service_loader gmproxy_1
ln -sf ../service_loader gmproxy_2
ln -sf ../service_loader gmproxy_3
ln -sf ../service_loader gmproxy_4
ln -sf ../service_loader gmproxy_5
ln -sf ../service_loader route
cd -

export ASAN_OPTIONS=include_if_exists=${DIR_file}/asan_options
export LSAN_OPTIONS=include_if_exists=${DIR_file}/lsan_options
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/ubsan_options
route()
{
   ${DIR_file}/binary/route --worldid=0 --start=60 --logpath=/data/log/route -d 
}

gmproxy_1()
{
    ${DIR_file}/binary/gmproxy_1 --worldid=0 --start=41 --logpath=/data/log/gmproxy1 -d 
}

gmproxy_2()
{
    ${DIR_file}/binary/gmproxy_2 --worldid=0 --start=42 --logpath=/data/log/gmproxy2 -d 
}

gmproxy_3()
{
${DIR_file}/binary/gmproxy_3 --worldid=0 --start=43 --logpath=/data/log/gmproxy3 -d 
}

gmproxy_4()
{
${DIR_file}/binary/gmproxy_4 --worldid=0 --start=44 --logpath=/data/log/gmproxy4 -d 
}

gmproxy_5()
{
${DIR_file}/binary/gmproxy_5 --worldid=0 --start=45 --logpath=/data/log/gmproxy5 -d 
}

if [ $1 ];
then
    $1;
else
    route;
    gmproxy_1;
    #gmproxy_2;
    #gmproxy_3;
    #gmproxy_4;
    #gmproxy_5;
fi