set -e
cd `dirname $0`
DIR_file=`pwd`

proto_name=$2
out_dir=$1
include_dir=$3
#echo ${proto_name}
#echo ${out_dir}

date1=`stat ${proto_name}.proto | grep -i Modify | awk '/Modify/{print $2" "$3}'`
date2=`stat ${proto_name}.pb.cc | grep -i Modify | awk '/Modify/{print $2" "$3}'`

t1=`date -d "$date1" +%s`
t2=`date -d "$date2" +%s`

if [ ! "$date2" ]; then
   ../../../depends/bin/protoc --proto_path=${include_dir} --cpp_out=${out_dir} ${proto_name}.proto 
elif  [ $t1 -gt $t2 ]; then
   ../../../depends/bin/protoc --proto_path=${include_dir} --cpp_out=${out_dir} ${proto_name}.proto 
fi


