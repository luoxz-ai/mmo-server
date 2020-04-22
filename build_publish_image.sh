set -e
cmd="./releasebuild.sh && cd bin && ./cp_depends_lib.sh && ./cp_release.sh"

ln -sf .dockerignore.publish .dockerignore

docker run --rm --name mmo-server-publish --volumes-from mmo-server-code --privileged --ulimit core=-1 --security-opt seccomp=unconfined mmo-server-depends sh -c "${cmd}"

docker build --rm -f "server-docker/mmo-server-publish.Dockerfile" -t mmo-server:$1 .