set -e
cd `dirname $0`
DIR_file=`pwd`

../../../depends/bin/protoc --proto_path=. --cpp_out=. sql_options/sql_options.proto 
