cp -rp ../../build/bin/release/* tools_bin/
cp -rp ../build/bin/release/service_loader .

mkdir -p libs
cd libs
cp -rp ../../build/lib/release/*.so .
cd ..
