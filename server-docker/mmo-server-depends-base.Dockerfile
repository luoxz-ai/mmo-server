FROM mmo-server-base:18.04

USER ubuntu
ADD --chown=ubuntu server-depends/ /data/mmorpg/server-depends
RUN chmod +x /data/mmorpg/server-depends/src/build_depends.sh
WORKDIR /data/mmorpg/server-depends/src

