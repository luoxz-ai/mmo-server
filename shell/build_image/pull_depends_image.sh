#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

docker pull ubuntu:18.04
docker pull mysql:5.7