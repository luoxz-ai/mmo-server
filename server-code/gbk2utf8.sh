path=$1
ext=$2

filelist=`find ${path} -path "*/*.${ext}" -print `
for file in $filelist
do
	echo $file
	enca -L zh_CN -x UTF-8 $file
  
done