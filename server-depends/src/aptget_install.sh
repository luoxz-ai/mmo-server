#sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y

apt-get update -y --fix-missing \
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
 ninja-build \
 libtool \
 language-pack-zh-hans \
 subversion \
 git \
 python \
 net-tools \
 linux-tools-generic \
 gdb \
 gcc-9 \
 g++-9 \
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
 libasan6 \
 tzdata \
 openssh-server