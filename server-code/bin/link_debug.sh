mkdir -p tools_bin
cd tools_bin
rm -f *
ln -sfv ../../build/bin/debug/* .

cd -
rm -f service_loader
ln -sfv ../build/bin/debug/service_loader .

mkdir -p libs
cd libs
rm -f *
ln -sfv ../../build/lib/debug/*.so .
cd ..
