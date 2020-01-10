FROM mmo-server-base:18.04

USER ubuntu
ADD --chown=ubuntu server-depends/ /data/mmorpg/server-depends
WORKDIR /data/mmorpg/server-depends/src

