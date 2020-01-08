cd jemalloc
make clean
cd ..

cd lua
make clean
cd ..

cd libevent
make clean
cd ..

rm cpp_redis/build/* -rf
rm protobuf/cmake/build/* -rf
rm tinyxml2/build/* -rf
rm curlpp/build/* -rf
rm xlnt/build/* -rf
rm fmt/build/* -rf
rm gflags/build/* -rf
rm snappy/build/* -rf
rm leveldb/build/* -rf
rm brpc/build/* -rf
