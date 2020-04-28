#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`


GLOBAL_IP=172.28.1.3
GLOBAL_PORT_START=17000
GLOBAL_OUT_IP=172.28.1.3

echo "create database global;" | docker exec -i mysql-global sh -c 'exec mysql -v -uroot -p"test12345"'
cat server-res/res/db/db_proto/globaldb.pb.sql | docker exec -i mysql-global sh -c 'exec mysql -v -uroot -p"test12345" global'


#cmd="mkdir -p server-res/res/db/db_proto.cmake && cd server-res/res/db/db_proto.cmake && cmake -DGLOBAL_IP=${GLOBAL_IP} -DGLOBAL_PORT_START=${GLOBAL_PORTD_START} -DGLOBAL_OUT_IP=${GLOBAL_OUT_IP} /data/mmorpg/server-res/res/db/db_protoinit_globalservice.sql.template"
cmd="mkdir -p .cmake_globaldb && cd .cmake_globaldb && cmake -DGLOBAL_IP=${GLOBAL_IP} -DGLOBAL_PORT_START=${GLOBAL_PORT_START} -DGLOBAL_OUT_IP=${GLOBAL_OUT_IP} -DGLOBALDB=ON /data/mmorpg/server-res/res/db && cat init_globalservice.sql"

docker exec -i mmo-server-build sh -c "${cmd}" |
docker exec -i mysql-global sh -c 'exec mysql -uroot -p"test12345" global'
echo "select * from tbld_servicedetail where worldid=0;" | docker exec -i mysql-global sh -c 'exec mysql -v -uroot -p"test12345" global'