FROM mmo-server-depends


ADD server-code/bin/ /data/mmorpg/server/server-code/bin/
USER root
WORKDIR /data/mmorpg/server/server-code/bin/
RUN ./cp_depends_lib.sh
RUN ./cp_release.sh

