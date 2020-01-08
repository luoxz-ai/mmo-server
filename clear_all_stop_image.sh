sudo docker rm $(sudo docker ps --all -q -f status=exited) 
sudo docker rm $(sudo docker ps --all -q -f status=created)