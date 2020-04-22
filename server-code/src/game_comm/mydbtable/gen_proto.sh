set -e
cd `dirname $0`
DIR_file=`pwd`

proto_name=$1
out_dir=${DIR_file}
include_dir=${DIR_file}
plugin_dir=$2
#echo ${proto_name}
#echo ${out_dir}

date1=`stat db_proto/${proto_name}.proto | grep -i Modify | awk '/Modify/{print $2" "$3}'`
date2=`stat db_proto/${proto_name}.pb.cc | grep -i Modify | awk '/Modify/{print $2" "$3}'`

t1=`date -d "$date1" +%s`
t2=`date -d "$date2" +%s`

if [ ! "$date2" ]; then
   ../../../depends/bin/protoc --plugin=${plugin_dir}/protoc-gen-sql --proto_path=${include_dir} --sql_out=${out_dir}/db --cpp_out=${out_dir} db_proto/${proto_name}.proto 
elif  [ $t1 -gt $t2 ]; then
   ../../../depends/bin/protoc --plugin=${plugin_dir}/protoc-gen-sql --proto_path=${include_dir} --sql_out=${out_dir}/db --cpp_out=${out_dir} db_proto/${proto_name}.proto 
fi

