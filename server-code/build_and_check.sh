mkdir -p build
cd build/
set -e
CC=gcc-7 CXX=g++-7 cmake .. -DDEBUG=ON -DTIDY=ON
make -j4
