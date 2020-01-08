FROM mmo-server-base:1804


ADD release/ /data/mmorpg/server/
USER root
WORKDIR /data/mmorpg/server/

