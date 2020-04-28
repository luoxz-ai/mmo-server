cd `dirname $0`
DIR_file=`pwd`

BINARIES=$1
sql_name=$2

date1=`stat db/db_proto/${sql_name}.pb.sql | grep -i Modify | awk '/Modify/{print $2" "$3}'`
date2=`stat ${sql_name}.h | grep -i Modify | awk '/Modify/{print $2" "$3}'`

t1=`date -d "$date1" +%s`
t2=`date -d "$date2" +%s`


if [ $t1 -gt $t2 ]; then
   ${BINARIES}/sql2cpp --input=db/db_proto/${sql_name}.pb.sql --outdir=./ --output=${sql_name} --format=clang-format-9
fi


