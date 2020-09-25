#!/bin/bash

cd `dirname $0`
DIR_file=`pwd`

source globaldb.env


# echo "create database ${SERVERINFO_MYSQL_NAME};" | docker exec -i mysql-global sh -c "exec mysql -v -uroot -p\"${MYSQL_PASSWD}\""
# set-e

cat server-res/res/db/db_proto/serverinfodb.pb.sql | docker exec -i mysql-global sh -c "exec mysql -v -uroot -p\"${MYSQL_PASSWD}\" ${SERVERINFO_MYSQL_NAME}"


echo "create database ${GLOBAL_MYSQL_NAME};" | docker exec -i mysql-global sh -c "exec mysql -v -uroot -p\"${MYSQL_PASSWD}\""
cat server-res/res/db/db_proto/globaldb.pb.sql | docker exec -i mysql-global sh -c "exec mysql -v -uroot -p\"${MYSQL_PASSWD}\" ${GLOBAL_MYSQL_NAME}"


#cmd="mkdir -p server-res/res/db/db_proto.cmake && cd server-res/res/db/db_proto.cmake && cmake -DGLOBAL_IP=${GLOBAL_IP} -DGLOBAL_PORT_START=${GLOBAL_PORTD_START} -DGLOBAL_OUT_IP=${GLOBAL_OUT_IP} /data/mmorpg/server-res/res/db/db_protoinit_globalservice.sql.template"
cmd="mkdir -p .cmake_globaldb && cd .cmake_globaldb && cmake -DGLOBALDB=ON /data/mmorpg/server-res/res/db && cat init_globalservice.sql"



docker exec \
-e GLOBAL_IP=${GLOBAL_IP} \
-e GLOBAL_PORT_START=${GLOBAL_PORT_START}  \
-e GLOBAL_OUT_IP=${GLOBAL_OUT_IP}  \
-e GLOBAL_MYSQL_IP=${GLOBAL_MYSQL_IP}  \
-e GLOBAL_MYSQL_NAME=${GLOBAL_MYSQL_NAME}  \
-e MYSQL_PASSWD=${MYSQL_PASSWD}  \
-e SERVERINFO_MYSQL_NAME=${SERVERINFO_MYSQL_NAME} \
-i mmo-server-build sh -c "${cmd}" | docker exec -i mysql-global sh -c "exec mysql -uroot -p\"${MYSQL_PASSWD}\" ${SERVERINFO_MYSQL_NAME}"

echo "select * from tbld_servicedetail where worldid=0;" | docker exec -i mysql-global sh -c "exec mysql -v -uroot -p\"${MYSQL_PASSWD}\" ${SERVERINFO_MYSQL_NAME}"




