# server-code

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
* CMake cmake额外依赖
* comm 全局公共库
* game_comm 游戏公共库
* server_share 服务间共享目录
* service 服务
* service_loader 服务加载器
* test 测试项目
* tools 工具项目

### src/comm目录
* basecode 基础代码库
* mydb 封装mysql访问
* network 网络库封装
* message_route 服务间通信组件

### src/game_comm目录
* gamemap 地图数据封装
* mydbtable 数据库 结构定义
* proto 消息结构和配置文件结构
* script 脚本管理封装

### src/service_loader目录
* service_loader 服务器装载器, 读取service.xml加载配置, 使用service_loader -d --start=1,11,12,13,14 启动服务 service_loader --stop=1,11,12,13,14 关闭服务

### src/service目录
* service_comm 服务封装
* rpcservice_comm rpc服务封装
#### 具体service
* socket_service 网关(玩家前端接入)
* world_service 账号登录,角色创建,角色选择, 帮派,组队,排行榜等全局唯一功能
* zone_service 游戏核心服务
* ai_service 一个zone对应一个ai服务
* gm_serivce 接入运营控制(一个区一个)
* dlog 数据日志上报
* market_serice 拍卖行服务
*   
* gmproxy_service 全局外部回调模块(根据外部http端口回调地址, 发送内部消息给对应区分的gm_service)
* route_service 全局服务控制模块
* maybe还有toplist_service排行榜服务等

### src/tools目录
* cxx2lua       解析项目中头文件上的export_lua标记，自动生成lua binding代码
* excel2pb      解析xlsx文件，转换为proto文件 
* pbbin2txt     解析proto文件，转换为文本
* sql2cpp       解析sql文件，转换为struct
* robotclient   压测机器人工具 使用lua编写实例


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
init_globaldb.sh
init_zonedb.sh zone_1.config


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
