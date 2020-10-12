#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

cd ../..
root_dir=`pwd`
cd -

docker run -d --restart always --privileged=true --network=network-mmo --ip=172.28.1.2 -e MYSQL_ROOT_PASSWORD=test12345 -v /${root_dir}/data/mysql-conf:/etc/mysql/mysql.conf.d -v /${root_dir}/data/mysql-global:/var/lib/mysql --name mysql-global mysql:5.7
docker run -d --restart always --privileged=true --network=network-mmo --ip=172.28.2.2 -e MYSQL_ROOT_PASSWORD=test12345 -v /${root_dir}/data/mysql-conf:/etc/mysql/mysql.conf.d -v /${root_dir}/data/mysql-zone1:/var/lib/mysql --name mysql-zone-1 mysql:5.7