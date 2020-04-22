#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

echo "select * from tbld_servicedetail;" | docker exec -i mysql-global sh -c 'exec mysql -uroot -p"3Killyou" global'