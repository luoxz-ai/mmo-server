FROM ubuntu:18.04

ADD mirror.list /root/mirror.list
RUN cp /root/mirror.list /etc/apt/sources.list
RUN apt-get update -y \
 && apt-get install -y \
 autoconf \
 binutils-dev \
 build-essential \
 curl \
 ccache \
 cmake \
 libcurl4-openssl-dev \
 libclang-3.9-dev \
 libtool \
 libmysqlclient-dev \
 libreadline6-dev \
 libssl-dev \
 language-pack-zh-hans \
 gcc-7 \
 g++-7 \
 subversion \
 sudo 
 
ENV LC_CTYPE=zh_CN.UTF-8 
 
RUN useradd --create-home --no-log-init --shell /bin/bash ubuntu
RUN adduser ubuntu sudo
RUN passwd -d ubuntu
USER ubuntu
WORKDIR /home/ubuntu
