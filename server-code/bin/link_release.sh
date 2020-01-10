ln -sf ../../build/bin/release/* tools_bin/
ln -sf ../build/bin/release/service_loader .

mkdir -p libs
cd libs
ln -sf ../../build/lib/release/*.so .
cd ..
