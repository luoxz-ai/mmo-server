pbname=$1
../../depends/bin/protoc --proto_path=../../src/proto/ ../../src/proto/config/${pbname}.proto -o ${pbname}.pb

lua_string="local pb = require \"libpb_lua\";assert(pb.loadfile \"${pbname}.pb\");local data = io.open (\"${pbname}.bytes\" , \"rb\");local msg = assert(pb.decode(\"${pbname}\",data));print(msg);"


echo ${lua_string} | ./lua

