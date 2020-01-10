docker rm $(sudo docker ps --all -q -f status=exited) 
docker rm $(sudo docker ps --all -q -f status=created)