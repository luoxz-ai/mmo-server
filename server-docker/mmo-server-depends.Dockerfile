FROM mmo-server-depends:base

USER ubuntu

RUN sudo /data/mmorpg/server-depends/src/build_depends.sh base;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh jemalloc;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh protobuf;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh cpp_redis;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh gflags;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh snappy;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh leveldb;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh brpc;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh libevent;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh tinyxml2;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh lua;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh curlpp;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh xlnt;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh fmt;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh libwebsockets;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh recastnavigation;
RUN sudo /data/mmorpg/server-depends/src/build_depends.sh box2d;


ENV CCACHE_SLOPPINESS pch_defines,time_macros
WORKDIR /data/mmorpg/server-code/
