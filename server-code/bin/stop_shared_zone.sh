#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

${DIR_file}/binary/shared_zone_1 --worldid=0 --stop=16,26  --logpath=/data/log/shared_zone_1
${DIR_file}/binary/shared_zone_2 --worldid=0 --stop=17,27  --logpath=/data/log/shared_zone_2
${DIR_file}/binary/shared_zone_3 --worldid=0 --stop=18,28  --logpath=/data/log/shared_zone_3
${DIR_file}/binary/shared_zone_4 --worldid=0 --stop=19,29  --logpath=/data/log/shared_zone_4
${DIR_file}/binary/shared_zone_5 --worldid=0 --stop=20,30  --logpath=/data/log/shared_zone_5
