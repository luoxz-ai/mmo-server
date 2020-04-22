FROM mmo-server-depends:base


ADD server-code/bin/ /data/mmorpg/server/server-code/bin/
ADD server-res/ /data/mmorpg/server/server-res

USER root
WORKDIR /data/mmorpg/server/server-code/bin/


