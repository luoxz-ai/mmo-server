# test-proj

## depends第三方库


* depends/src/cpp_redis redis的client
* depends/src/jemalloc 内存分配
* depends/src/libevent 网络
* depends/src/lua 脚本
* depends/src/mysql-connector-c mysql访问
* depends/src/protobuf 协议库
* depends/src/tineyxml2 xml读取库
* depends/src/rapidjson json读取库
* depends/src/curlpp curl库

## src目录

* share 公共头文件
* proto 消息结构和配置文件结构
* basecode 基础代码库
* network 网络库封装
* service_comm 服务器集群化互联封装
* mydb 封装mysql访问
* mydbtable mysql数据库结构
* mapdata 场景数据读取
* script 脚本封装
* service_loader 服务器装载器, 读取service.xml加载配置, 使用service_loader -d --start=1,11,12,13,14 启动服务 service_loader --stop=1,11,12,13,14 关闭服务

* socket_service 网关(玩家前端接入)
* world_service 账号登录,角色创建,角色选择, 帮派,组队,排行榜等全局唯一功能
* zone_service 游戏核心服务
* ai_service 一个zone对应一个ai服务
* gm_serivce 接入运营控制(一个区一个)
* gmproxy_service 全局外部回调模块(根据外部http端口回调地址, 发送内部消息给对应区分的gm_service)
* route_service 全局服务控制模块
* dlog 彩虹数据日志上报
* makret_serice 拍卖行服务

## 建立vcproj文件
* 在windows上执行make_proj.bat

## 执行步骤
```
#编译第三方库
cd depends/src
./build_depends.sh

#编译项目
cd ../..
./build.sh

#挂接文件
cd bin
link_debug.sh 或者 link_release.sh
link_depends_lib.sh


#初始化数据库
cd db
mysql -h 127.0.0.1 -u root --password=123456 global < globaldb.sql
mysql -h 127.0.0.1 -u root --password=123456 global < init_globalservice.sql
mysql -h 127.0.0.1 -u root --password=123456 global < add_new_server.sql
mysql -h 127.0.0.1 -u root --password=123456 zone_1001 < gamedb.sql


#启动global
./start_global.sh
#分离式启动global
./start_all_global.sh
#启动zone
./start_zone.sh 1001
#分离式启动zone进程
./start_zone_separat.sh 1001

也可使用
./service_loader --config=service.xml --start -d
```


#注意事项

所有的serivce使用动态库的方式实现而不是可执行文件
这样方便使用service_loader进程来分离或合并加载不同的service

为防止因为同名类/同名变量导致代码覆盖的问题, 
world_service,socket_service,zone_service,gm_serivce,makret_serice
均只导出一个函数ServiceCreate
其他符号均为隐藏
