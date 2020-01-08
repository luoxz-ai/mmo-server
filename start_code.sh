sudo docker start -ia mmo-server-build
sudo docker run -it --name mmo-server-build --volumes-from mmo-server-code mmo-server-depends bash