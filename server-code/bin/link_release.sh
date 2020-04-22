mkdir -p tools_bin
cd tools_bin
rm -f *
ln -sfv ../../build/bin/release/* .

cd -
rm -f service_loader
ln -sfv ../build/bin/release/service_loader .

mkdir -p libs
cd libs
rm -f *
ln -sfv ../../build/lib/release/*.so .
cd ..
