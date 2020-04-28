#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`


source $1

echo "create database ${ZONE_NAME};" | docker exec -i ${MYSQL_DOCKER_NAME} sh -c 'exec mysql -v -uroot -p"test12345"'

sql_cmd="exec mysql -v -uroot -p'test12345' ${ZONE_NAME}"
cat server-res/res/db/db_proto/gamedb.pb.sql | docker exec -i ${MYSQL_DOCKER_NAME} sh -c "${sql_cmd}"



#cmd="m4 -DZONE_ID=${ZONE_ID} -DZONE_NAME=${ZONE_NAME} -DZONE_IP=${ZONE_IP} \
#-DPORT_START=${PORT_START} -DZONE_OUT_IP=${ZONE_OUT_IP} -DOUT_PORT_START=${OUT_PORT_START} -DZONE_MYSQL_IP=${ZONE_MYSQL_IP} \
#-DMYSQL_PASSWD=${MYSQL_PASSWD} -DZONE_OPEN_TIME=${ZONE_OPEN_TIME} /data/mmorpg/server-res/res/db/db_protoadd_new_server.sql.template"

cmd="mkdir -p .cmake_zonedb && \
cd .cmake_zonedb && \
cmake -DZONE_ID=${ZONE_ID} -DZONE_NAME=${ZONE_NAME} -DZONE_IP=${ZONE_IP} \
-DPORT_START=${PORT_START} -DZONE_OUT_IP=${ZONE_OUT_IP} -DOUT_PORT_START=${OUT_PORT_START} -DOUT_PORT_END=${OUT_PORT_END} -DZONE_MYSQL_IP=${ZONE_MYSQL_IP} \
-DMYSQL_PASSWD=${MYSQL_PASSWD} -DZONE_OPEN_TIME=${ZONE_OPEN_TIME} -DZONEDB=${ZONE_ID} /data/mmorpg/server-res/res/db && \
cat zone_init_${ZONE_ID}.sql"

docker exec -i mmo-server-build sh -c "${cmd}" | 
docker exec -i mysql-global sh -c 'exec mysql -v -uroot -p"test12345" global'
echo "select * from tbld_servicedetail where worldid=${ZONE_ID};" | docker exec -i mysql-global sh -c 'exec mysql -uroot -p"test12345" global'