mkdir -p tools_bin
cd tools_bin
ln -sf ../../build/bin/debug/* .

cd -
ln -sf ../build/bin/debug/service_loader .

mkdir -p libs
cd libs
ln -sf ../../build/lib/debug/*.so .
cd ..
