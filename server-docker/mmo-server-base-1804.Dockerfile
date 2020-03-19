FROM ubuntu:18.04

ADD mirror.list /root/mirror.list
RUN cp /root/mirror.list /etc/apt/sources.list
RUN apt-get update -y --fix-missing \
 && apt-get install -y --fix-missing \
 apt-utils \
 && apt-get install -y --fix-missing \
 sudo \
 vim \
 less \
 autoconf \
 binutils-dev \
 build-essential \
 curl \
 ccache \
 cmake \
 libtool \
 language-pack-zh-hans \
 subversion \
 git \
 python \
 net-tools \
 linux-tools-generic \
 gdb \
 gcc-7 \
 g++-7 \
 clang-9 \
 clang-format-9 \
 clang-tidy-9 \
 libclang-9-dev \
 lldb-9 \
 lld-9 \
 libc++-9-dev \
 libmysqlclient-dev \
 mysql-client \
 libcurl4-openssl-dev \
 libreadline6-dev \
 libssl-dev \
 libasan5 


ENV LC_CTYPE=zh_CN.UTF-8 
ENV TZ=Asia/Shanghai
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN echo "/corefile/core-%e-%p-%t" > /proc/sys/kernel/core_pattern

RUN useradd --create-home --no-log-init --shell /bin/bash ubuntu
RUN adduser ubuntu sudo
RUN passwd -d ubuntu
USER ubuntu
WORKDIR /home/ubuntu




