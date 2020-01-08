FROM mmo-server-base:1804

USER ubuntu
ADD --chown=ubuntu server-depends/ /data/mmorpg/server-depends
WORKDIR /data/mmorpg/server-depends/src

RUN /data/mmorpg/server-depends/src/build_depends.sh base;
RUN /data/mmorpg/server-depends/src/build_depends.sh jemalloc;
RUN /data/mmorpg/server-depends/src/build_depends.sh protobuf;
RUN /data/mmorpg/server-depends/src/build_depends.sh cpp_redis;
RUN /data/mmorpg/server-depends/src/build_depends.sh gflags;
RUN /data/mmorpg/server-depends/src/build_depends.sh snappy;
RUN /data/mmorpg/server-depends/src/build_depends.sh leveldb;
RUN /data/mmorpg/server-depends/src/build_depends.sh brpc;
RUN /data/mmorpg/server-depends/src/build_depends.sh libevent;
RUN /data/mmorpg/server-depends/src/build_depends.sh tinyxml2;
RUN /data/mmorpg/server-depends/src/build_depends.sh lua;
RUN /data/mmorpg/server-depends/src/build_depends.sh curlpp;
RUN /data/mmorpg/server-depends/src/build_depends.sh xlnt;
RUN /data/mmorpg/server-depends/src/build_depends.sh fmt;
RUN /data/mmorpg/server-depends/src/build_depends.sh libwebsockets;


ENV CCACHE_SLOPPINESS pch_defines,time_macros
WORKDIR /data/mmorpg/server-code/
