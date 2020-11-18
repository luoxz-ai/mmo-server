FROM mmo-server-depends:base

USER ubuntu

RUN /data/mmorpg/server-depends/src/build_all.sh;
RUN /data/mmorpg/server-depends/src/clear_build.sh;

ENV CCACHE_SLOPPINESS pch_defines,time_macros
ENV CCACHE_COMPRESS 1
ENV CCACHE_DIR /data/mmorpg/server-code/build/.ccache
WORKDIR /data/mmorpg/server-code/
