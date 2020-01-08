mkdir -p build
cd build/
set -e
CC=gcc-7 CXX=g++-7 cmake .. -DDEBUG=OFF -DTIDY=OFF
make -j4
