rm -f tools_bin/*
rm -f service_loader

mkdir -p tools_bin
cp -vrdf ../build/bin/release/* tools_bin/
cp -vrdf ../build/bin/release/service_loader .

mkdir -p libs
cd libs
rm -rf *
cp -vrdf ../../build/lib/release/*.so .
cd -
