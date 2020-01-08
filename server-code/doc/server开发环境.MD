#申请ubuntu虚拟机


cd /data
#安装svn
sudo apt-get install subversion

#clone仓库
svn checkout http://svn.ids111.com/studios/nebula-art/mmorpg/trunk/server mmo-server

#修改所有.sh文件为可执行
sudo find -name "*.sh" -type f -exec chmod +x {} \;

cd depends/src
#安装依赖
./aptget_install.sh 
#编译第三方库
./build_depends.sh
cd ../..


#编译代码
./build.sh



#windows安装TortoiseSVN
#执行代码目录make_proj.bat会自动建立vc项目



#初始化数据库
cd db
mysql -h 127.0.0.1 -u root --password=123456 global < globaldb.sql
mysql -h 127.0.0.1 -u root --password=123456 global < init_globalservice.sql
mysql -h 127.0.0.1 -u root --password=123456 global < add_new_server.sql
mysql -h 127.0.0.1 -u root --password=123456 zone_1001 < gamedb.sql



#启动global
./start_all_global.sh
#启动zone
./start_zone.sh 1001
#分离式启动zone进程
./start_zone_separat.sh 1001

也可使用
./service_loader --config=service.xml --start -d




