# mmo-server
mmo-server


# 目录
* sercer-code 代码
* server-depends 外部依赖库
* server-docker docker配置文件
* server-res 资源目录
* share-code client-server共享文件
* . 快捷shell

# 快捷shell
* install_docker.sh           linux安装docker
* pull_depends_image.sh       拉取依赖docker
* build_base1804_image.sh     构建ubuntu18.04基础docker
* build_mmo-server-depends.sh 构建外部依赖库
* build_publish_image.sh      对外发布
* 
* create_network.sh           创建docker实验网络
* start_mysql.sh              启动global，zone的数据库docker
* link_codedir.sh             将代码目录映射给docker数据卷
* start_code.sh               启动编译用docker
* 
* init_globaldb.sh            初始化global数据库
* init_newzonedb.sh zone_.config  初始化zone数据库
* show_serverdetail.sh        显示数据库里的服务路由表
* start_all_global.sh         启动global服务
