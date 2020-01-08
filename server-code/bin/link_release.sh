ln -sf ../build/bin/release/* .
mkdir -p libs
cd libs
ln -sf ../../build/lib/release/*.so .
cd ..
