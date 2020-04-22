#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

path=$1
ext=$2

filelist=`find ${path} -path "*/*.${ext}" -print `
for file in $filelist
do
	echo $file
	enca -L zh_CN -x UTF-8 $file
  
done