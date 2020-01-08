#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`


ln -sf service_loader shared_zone_1
ln -sf service_loader shared_zone_2
ln -sf service_loader shared_zone_3
ln -sf service_loader shared_zone_4
ln -sf service_loader shared_zone_5


${DIR_file}/shared_zone_1 --worldid=0 --start=16,26 -d --logpath=shared_zone_1
${DIR_file}/shared_zone_2 --worldid=0 --start=17,27 -d --logpath=shared_zone_2
${DIR_file}/shared_zone_3 --worldid=0 --start=18,28 -d --logpath=shared_zone_3
${DIR_file}/shared_zone_4 --worldid=0 --start=19,29 -d --logpath=shared_zone_4
${DIR_file}/shared_zone_5 --worldid=0 --start=20,30 -d --logpath=shared_zone_5