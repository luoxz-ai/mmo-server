cd jemalloc
make clean
cd ..

cd lua
make clean
cd ..

cd libevent
make clean
cd ..

rm protobuf/build/* -rf
rm cpp_redis/build/* -rf
rm gflags/build/* -rf
rm snappy/build/* -rf
rm leveldb/build/* -rf
rm brpc/build/* -rf

rm tinyxml2/build/* -rf
rm curlpp/build/* -rf
rm xlnt/build/* -rf
rm fmt/build/* -rf
rm libwebsockets/build/* -rf
rm recastnavigation/build/* -rf
rm box2d/build/* -rf


