set -e
cd `dirname $0`
DIR_file=`pwd`

./gen_proto.sh .  ts_cmd .
./gen_proto.sh .  zone_service .
./gen_proto.sh .  world_service .
./gen_proto.sh .  server_side .

filelist=`ls config/*.proto`
for file in $filelist
do
  ./gen_proto.sh . ${file%\.*} .
done


filelist=`ls common/*.proto`
for file in $filelist
do
  ./gen_proto.sh . ${file%\.*} .
done
