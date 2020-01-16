ZONE_ID=1
ZONE_NAME=zone_1
MYSQL_DOCKER_NAME=mysql-zone-1
ZONE_IP=172.28.2.3
PORT_START=17200
OUT_PORT_START=18000
ZONE_MYSQL_IP=172.28.2.2
MYSQL_PASSWD=3Killyou
ZONE_OPEN_TIME="\"NOW()\""

echo "create database ${ZONE_NAME};" | docker exec -i ${MYSQL_DOCKER_NAME} sh -c 'exec mysql -uroot -p"3Killyou"'

sql_cmd="exec mysql -uroot -p'3Killyou' ${ZONE_NAME}"
cat server-code/bin/res/db/gamedb.sql | docker exec -i ${MYSQL_DOCKER_NAME} sh -c "${sql_cmd}"



cmd="m4 -DZONE_ID=${ZONE_ID} -DZONE_NAME=${ZONE_NAME} -DZONE_IP=${ZONE_IP} \
-DPORT_START=${PORT_START} -DOUT_PORT_START=${OUT_PORT_START} -DZONE_MYSQL_IP=${ZONE_MYSQL_IP} \
-DMYSQL_PASSWD=${MYSQL_PASSWD} -DZONE_OPEN_TIME=${ZONE_OPEN_TIME} /data/mmorpg/server-code/bin/res/db/add_new_server.sql.template"


docker exec -i mmo-server-build sh -c "${cmd}" | 
docker exec -i mysql-global sh -c 'exec mysql -uroot -p"3Killyou" global'