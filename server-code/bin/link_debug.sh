ln -sf ../build/bin/debug/* .
mkdir -p libs
cd libs
ln -sf ../../build/lib/debug/*.so .
cd ..
