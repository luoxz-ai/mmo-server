set -e
cd `dirname $0`
DIR_file=`pwd`


build_depends.sh base;
build_depends.sh jemalloc;
build_depends.sh protobuf;
build_depends.sh cpp_redis;
build_depends.sh gflags;
build_depends.sh snappy;
build_depends.sh leveldb;
build_depends.sh brpc;
build_depends.sh libevent;
build_depends.sh tinyxml2;
build_depends.sh lua;
build_depends.sh curlpp;
build_depends.sh xlnt;
build_depends.sh fmt;
build_depends.sh libwebsockets;
build_depends.sh recastnavigation;
build_depends.sh box2d;