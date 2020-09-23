#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

export ASAN_OPTIONS=include_if_exists=${DIR_file}/options_asan
export LSAN_OPTIONS=include_if_exists=${DIR_file}/options_lsan
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/options_ubsan
export TSAN_OPTIONS=include_if_exists=${DIR_file}/options_tsan

route()
{
    ${DIR_file}/binary/route --worldid=0 --stop=ROUTE_SERVICE-0 --logpath=/data/log/global/route 
}

gmproxy_1()
{
    ${DIR_file}/binary/gmproxy_1 --worldid=0 --stop=GM_PROXY_SERVICE-1 --logpath=/data/log/global/gmproxy1 
}

gmproxy_2()
{
    ${DIR_file}/binary/gmproxy_2 --worldid=0 --stop=GM_PROXY_SERVICE-2 --logpath=/data/log/global/gmproxy2 
}

gmproxy_3()
{
${DIR_file}/binary/gmproxy_3 --worldid=0 --stop=GM_PROXY_SERVICE-3 --logpath=/data/log/global/gmproxy3 
}

gmproxy_4()
{
${DIR_file}/binary/gmproxy_4 --worldid=0 --stop=GM_PROXY_SERVICE-4 --logpath=/data/log/global/gmproxy4 
}

gmproxy_5()
{
${DIR_file}/binary/gmproxy_5 --worldid=0 --stop=GM_PROXY_SERVICE-5 --logpath=/data/log/global/gmproxy5 
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
