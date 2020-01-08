mkdir -p libs
cd libs
ln -s ../../depends/lib/*.so* .

cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.24 .
ln -s libstdc++.so.6.0.24 libstdc++.so.6

cd ..
ln -s ../depends/bin/* .
