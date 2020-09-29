#!/bin/bash

cd `dirname $0`
DIR_file=`pwd`


env_file=$1
if [ ! -n "$1" ] ;then  
    echo "evn_file not find";
    exit;
fi 

MYSQL_PASSWD=test12345
source $env_file

create_db()
{
echo "create database IF NOT EXISTS ${ZONE_NAME};" | docker exec -i ${MYSQL_DOCKER_NAME} sh -c "exec mysql -v -uroot -p\"${MYSQL_PASSWD}\""

set -e
sql_cmd="exec mysql -v -uroot -p\"${MYSQL_PASSWD}\" ${ZONE_NAME}"
cat server-res/res/db/db_proto/gamedb.pb.sql | docker exec -i ${MYSQL_DOCKER_NAME} sh -c "${sql_cmd}"
}



insert_serverinfo()
{
#cmd="m4 -DZONE_ID=${ZONE_ID} -DZONE_NAME=${ZONE_NAME} -DZONE_IP=${ZONE_IP} \
#-DPORT_START=${PORT_START} -DZONE_OUT_IP=${ZONE_OUT_IP} -DOUT_PORT_START=${OUT_PORT_START} -DZONE_MYSQL_IP=${ZONE_MYSQL_IP} \
#-DMYSQL_PASSWD=${MYSQL_PASSWD} -DZONE_OPEN_TIME=${ZONE_OPEN_TIME} /data/mmorpg/server-res/res/db/db_protoadd_new_server.sql.template"

cmd="mkdir -p .cmake_zonedb && \
cd .cmake_zonedb && \
cmake /data/mmorpg/server-res/res/db && \
cat zone_init_${ZONE_ID}.sql"

docker exec \
-e ZONE_ID=${ZONE_ID} \
-e ZONE_NAME=${ZONE_NAME} \
-e MYSQL_DOCKER_NAME=${MYSQL_DOCKER_NAME} \
-e ZONE_IP=${ZONE_IP} \
-e PORT_START=${PORT_START} \
-e ZONE_OUT_IP=${ZONE_OUT_IP} \
-e OUT_PORT_START=${OUT_PORT_START} \
-e OUT_PORT_END=${OUT_PORT_END} \
-e ZONE_MYSQL_URL=${ZONE_MYSQL_URL} \
-e ZONE_OPEN_TIME=${ZONE_OPEN_TIME} \
 -i mmo-server-build  sh -c "${cmd}" | docker exec -i mysql-global sh -c "exec mysql -v -uroot -p\"${MYSQL_PASSWD}\" serverinfo"
}

show_serverinfodb()
{
    echo "select * from tbld_servicedetail where worldid=${ZONE_ID};" | docker exec -i mysql-global sh -c "exec mysql -uroot -p\"${MYSQL_PASSWD}\" serverinfo"
}

if [ $2 ];
then
    $2;
else
    create_db;
    insert_serverinfo;
    show_serverinfodb;
    
fi
