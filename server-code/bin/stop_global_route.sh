#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

export ASAN_OPTIONS=include_if_exists=${DIR_file}/options_asan
export LSAN_OPTIONS=include_if_exists=${DIR_file}/options_lsan
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/options_ubsan
export TSAN_OPTIONS=include_if_exists=${DIR_file}/options_tsan

route()
{
    ${DIR_file}/binary/route --worldid=0 --stop=60 --logpath=/data/log/route 
}

gmproxy_1()
{
    ${DIR_file}/binary/gmproxy_1 --worldid=0 --stop=41 --logpath=/data/log/gmproxy1 
}

gmproxy_2()
{
    ${DIR_file}/binary/gmproxy_2 --worldid=0 --stop=42 --logpath=/data/log/gmproxy2 
}

gmproxy_3()
{
${DIR_file}/binary/gmproxy_3 --worldid=0 --stop=43 --logpath=/data/log/gmproxy3 
}

gmproxy_4()
{
${DIR_file}/binary/gmproxy_4 --worldid=0 --stop=44 --logpath=/data/log/gmproxy4 
}

gmproxy_5()
{
${DIR_file}/binary/gmproxy_5 --worldid=0 --stop=45 --logpath=/data/log/gmproxy5 
}

if [ $1 ];
then
    $1;
else
    route;
    gmproxy_1;
    gmproxy_2;
    gmproxy_3;
    gmproxy_4;
    gmproxy_5;   
fi
