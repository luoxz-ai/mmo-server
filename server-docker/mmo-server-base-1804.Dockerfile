FROM ubuntu:18.04

ADD mirror.list /root/mirror.list
RUN cp /root/mirror.list /etc/apt/sources.list
RUN apt-get update -y --fix-missing \
 && apt-get install -y \
 sudo \
 vim \
 less \
 autoconf \
 binutils-dev \
 build-essential \
 curl \
 ccache \
 cmake \
 libcurl4-openssl-dev \
 libtool \
 libmysqlclient-dev \
 libreadline6-dev \
 libssl-dev \
 language-pack-zh-hans \
 subversion \
 git \
 gdb \
 python \
 net-tools \
 mysql-client \
 linux-tools-common \
 linux-tools-generic \
 && apt-get install -y \
 gcc-7 \
 g++-7 \
 clang-9 \
 clang-format-9 \
 clang-tidy-9 \
 lldb-9 \
 libclang-9-dev \
 libc++-9-dev 
 
 
ENV LC_CTYPE=zh_CN.UTF-8 
ENV TZ=Asia/Shanghai
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN useradd --create-home --no-log-init --shell /bin/bash ubuntu
RUN adduser ubuntu sudo
RUN passwd -d ubuntu
USER ubuntu
WORKDIR /home/ubuntu



