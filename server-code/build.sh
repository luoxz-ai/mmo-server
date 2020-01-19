mkdir -p build
cd build/
set -e
CC=clang-9 CXX=clang++-9 cmake .. -DDEBUG=ON
make -j4
