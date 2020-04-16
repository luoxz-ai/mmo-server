set -e
cd `dirname $0`
DIR_file=`pwd`

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

filelist=`ls msg/*.proto`
for file in $filelist
do
  ./gen_proto.sh . ${file%\.*} .
done

filelist=`ls server_msg/*.proto`
for file in $filelist
do
  ./gen_proto.sh . ${file%\.*} .
done
