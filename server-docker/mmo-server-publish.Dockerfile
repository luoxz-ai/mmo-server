FROM mmo-server-base:18.04


ADD release/ /data/mmorpg/server/
USER root
WORKDIR /data/mmorpg/server/

