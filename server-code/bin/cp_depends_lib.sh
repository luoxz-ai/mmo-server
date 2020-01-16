mkdir -p depends
cd depends
cp -rp ../../depends/lib/*.so* .

cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.24 .
cp -rp libstdc++.so.6.0.24 libstdc++.so.6

cd ..
#ln -s ../depends/bin/* .
